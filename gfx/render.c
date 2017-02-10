#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/bytes.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/math/vec4.h>

struct render_queue *render_queue_alloc(struct list_head *stage, struct shader *pipeline)
{
        struct render_queue *p = smalloc(sizeof(struct render_queue));
        INIT_LIST_HEAD(&p->content_list);
        INIT_LIST_HEAD(&p->stage_head);
        p->pipeline = pipeline;
        list_add_tail(&p->stage_head, stage);
        return p;
}

void render_queue_free(struct render_queue *queue)
{
        struct list_head *p, *n;
        list_for_each_safe(p, n, &queue->content_list) {
                struct render_content *c = (struct render_content *)
                        ((void*)p - offsetof(struct render_content, queue_head));
                render_content_free(c);
        }
        /* detach and deallocate */
        list_del(&queue->stage_head);
        sfree(queue);
}

struct render_content *render_content_alloc(struct render_queue *queue,
        struct array *buffers[BUFFERS], u16 vertice, u16 max_instances)
{
        struct render_content *p = smalloc(sizeof(struct render_content));
        list_add_tail(&p->queue_head, &queue->content_list);
        i16 i;
        for_i(i, BUFFERS) {
                p->groups[i] = device_buffer_group_alloc();
                struct device_buffer **buffer;
                array_for_each(buffer, buffers[i]) {
                        device_buffer_group_add(p->groups[i], *buffer);
                }
#if GFX == OGL
                shader_setup_group(queue->pipeline, p->groups[i]);
#endif
        }
        p->textures             = array_alloc(sizeof(struct texture *), ORDERED);
        p->vertice              = vertice;
        p->max_instances        = max_instances;
        p->current_instances    = 0;
        INIT_LIST_HEAD(&p->node_list);
        INIT_LIST_HEAD(&p->pending_updaters);
        return p;
}

void render_content_set_texture(struct render_content *content, u16 index, struct texture *t)
{
        while(content->textures->len <= index) {
                struct texture *p = NULL;
                array_push(content->textures, &p);
        }
        struct texture *p = array_get(content->textures, struct texture *, index);
        if(p) texture_free(p);
        t->ref++;
        array_set(content->textures, index, &t);
}

void render_content_free(struct render_content *content)
{
        i16 i;
        for_i(i, BUFFERS) {
                device_buffer_group_free(content->groups[i]);
        }
        array_deep_free_safe(content->textures, struct texture *, texture_free);
        /* detach and deallocate */
        list_del(&content->queue_head);
        sfree(content);
}

struct node_data *node_data_alloc()
{
        struct node_data *p = smalloc(sizeof(struct node_data));
        p->frames = 0;
        p->buffer_id = 0;
        p->data = bytes_alloc();
        return p;
}

void node_data_set(struct node_data *p, u8 bid, void *bytes, u32 len)
{
        p->frames = BUFFERS;
        p->buffer_id = bid;
        p->data->len = 0;
        bytes_cat(p->data, bytes, len);
}

void node_data_free(struct node_data *p)
{
        bytes_free(p->data);
        sfree(p);
}

struct node *node_alloc(struct render_content *host)
{
        struct node *p = smalloc(sizeof(struct node));
        p->content_index = host->current_instances;
        p->host = host;

        host->current_instances++;
        list_add_tail(&p->content_head, &host->node_list);

        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->updater_head);

        p->pending_datas = array_alloc(sizeof(struct node_data *), UNORDERED);
        p->datas         = array_alloc(sizeof(struct node_data *), ORDERED);
        u8 i;
        for_i(i, host->groups[0]->buffers->len) {
                struct node_data *nd = node_data_alloc();
                nd->buffer_id = i;
                array_push(p->datas, &nd);
        }

        return p;
}

void node_set_data(struct node *p, u8 index, void *bytes, u32 len)
{
        struct node_data *d = array_get(p->datas, struct node_data *, index);
        if(d->frames == 0) array_push(p->pending_datas, &d);
        node_data_set(d, index, bytes, len);

        if(list_singular(&p->updater_head)) {
                list_add_tail(&p->updater_head, &p->host->pending_updaters);
        }
}

void node_add_child(struct node *p, struct node *child)
{
        list_del(&child->tree_head);
        list_add_tail(&child->tree_head, &p->children);
}

void node_free(struct node *p)
{
        /*
         * detach p from host
         * node having biggest index in host will change to p index
         * in order to maintain right host instances
         */
        if(!list_singular(&p->content_head)) {
                struct node *tail = (struct node *)
                        ((void*)p->host->node_list.prev - offsetof(struct node, content_head));
                if(tail != p) {
                        struct node_data **data;
                        array_for_each(data, tail->datas) {
                                if((*data)->data->len) {
                                        node_set_data(tail, (*data)->buffer_id,
                                                (*data)->data->ptr, (*data)->data->len);
                                }
                        }
                        tail->content_index = p->content_index;
                }
                list_del(&p->content_head);
                p->host->current_instances--;
        }
        list_del(&p->updater_head);
        /* detach from parent-child tree */
        list_del(&p->tree_head);

        /* clear data */
        array_free(p->pending_datas);
        array_deep_free(p->datas, struct node_data *, node_data_free);

        /* deallocate children */
        struct list_head *child_head, *child_head_next;
        list_for_each_safe(child_head, child_head_next, &p->children) {
                struct node *child_node = (struct node *)
                        ((void*)child_head - offsetof(struct node, tree_head));
                node_free(child_node);
        }

        sfree(p);
}

struct render_stage *render_stage_alloc(struct renderer *renderer)
{
        struct render_stage *p = smalloc(sizeof(struct render_stage));
        INIT_LIST_HEAD(&p->stencil_queue_list);
        INIT_LIST_HEAD(&p->content_queue_list);
        list_add_tail(&p->renderer_head, &renderer->stage_list);
        return p;
}

void render_stage_free(struct render_stage *p)
{
        struct list_head *head, *next;
        /* free stencil queues */
        list_for_each_safe(head, next, &p->stencil_queue_list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                render_queue_free(queue);
        }
        /* free content queues */
        list_for_each_safe(head, next, &p->content_queue_list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                render_queue_free(queue);
        }
        /* detach and deallocate */
        list_del(&p->renderer_head);
        sfree(p);
}

struct renderer *renderer_alloc()
{
        struct renderer *p = smalloc(sizeof(struct renderer));
        INIT_LIST_HEAD(&p->stage_list);
        p->color = smalloc(sizeof(union vec4));
        *p->color = vec4((float[4]){0, 0, 0, 1});
        p->pass = NULL;
        return p;
}

void renderer_set_color(struct renderer *p, union vec4 *color)
{
        if(color) {
                if(!p->color) {
                        p->color = smalloc(sizeof(union vec4));
                }
                *p->color = *color;
        }
        else {
                sfree(p->color);
                p->color = NULL;
        }
}

void renderer_free(struct renderer *p)
{
        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->stage_list) {
                struct render_stage *stage = (struct render_stage *)
                        ((void*)head - offsetof(struct render_stage, renderer_head));
                render_stage_free(stage);
        }
        if(p->color) sfree(p->color);
        if(p->pass) p->pass->del(p->pass);
        sfree(p);
}