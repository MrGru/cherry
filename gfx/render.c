#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/graphic/device_buffer.h>

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

struct render_content *render_content_alloc(struct render_queue *queue, struct mesh *mesh)
{
        struct render_content *p = smalloc(sizeof(struct render_content));
        list_add_tail(&p->queue_head, &queue->content_list);
        i16 i;
        for_i(i, BUFFERS) {
                p->groups[i] = device_buffer_group_alloc();
                u8 *type;
                array_for_each(type, queue->pipeline->mesh_types) {
                        struct device_buffer *buffer = map_get(mesh->buffers[i],
                                struct device_buffer *, type, sizeof(*type));
                        device_buffer_group_add(p->groups[i], buffer);
                }
#if GFX == OGL
                shader_setup_group(queue->pipeline, p->groups[i]);
#endif
        }
        p->textures = array_alloc(sizeof(struct texture *), ORDERED);
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
        return p;
}

void renderer_free(struct renderer *p)
{
        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->stage_list) {
                struct render_stage *stage = (struct render_stage *)
                        ((void*)head - offsetof(struct render_stage, renderer_head));
                render_stage_free(stage);
        }
        sfree(p);
}
