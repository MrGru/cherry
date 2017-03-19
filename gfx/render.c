/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/bytes.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/math/vec4.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/texture.h>
#include <cherry/xml/xml.h>
#include <cherry/string.h>

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
        list_del_init(&queue->stage_head);
        sfree(queue);
}

struct render_content *render_content_alloc(struct render_queue *queue,
        struct array *buffers[BUFFERS], u32 vertice, u32 max_instances, u32 instance_multiple)
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
                shader_setup_group(queue->pipeline, p->groups[i]);
        }
        p->textures             = array_alloc(sizeof(struct texture *), ORDERED);
        p->atlases              = map_alloc(sizeof(struct map *));
        p->vertice              = vertice;
        p->max_instances        = max_instances;
        p->instance_multiple    = instance_multiple;
        p->current_instances    = 0;
        p->depth_test           = 0;
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
        if(t) t->ref++;
        array_set(content->textures, index, &t);
}

struct texture *render_content_get_texture(struct render_content *content, u16 index)
{
        if(content->textures->len > index) {
                return array_get(content->textures, struct texture *, index);
        }
        return NULL;
}

void render_content_set_bitmap_font(struct render_content *content, u16 index, char *font_path)
{
        struct map *font                        = map_alloc(sizeof(struct font_frame));
        struct xml_element *xml                 = xml_parse(font_path);

        i16 size;
        i16 line_height;
        i16 base;
        i16 tex_width;
        i16 tex_height;

        /*
         * read size and common info
         */
        struct xml_element *info                = xml_find(xml, "info", 0);
        size                                    = atoi(xml_find_attribute(info, "size")->value->ptr);

        struct xml_element *common              = xml_find(xml, "common", 0);
        line_height                             = atoi(xml_find_attribute(common, "lineHeight")->value->ptr);
        base                                    = atoi(xml_find_attribute(common, "base")->value->ptr);
        tex_width                               = atoi(xml_find_attribute(common, "scaleW")->value->ptr);
        tex_height                              = atoi(xml_find_attribute(common, "scaleH")->value->ptr);

        /*
         * load texture
         */
        struct string *tex_name                 = string_alloc_chars(font_path, strlen(font_path));
        int i;
        back_i(i, tex_name->len) {
                if(tex_name->ptr[i] == '/') {
                        tex_name->len -= (tex_name->len - 1 - i);
                        string_cat_string(tex_name, xml_find_attribute(xml_find(xml, "page", 0), "file")->value);
                        break;
                }
        }
        render_content_set_texture(content, index, texture_alloc_file(tex_name->ptr));
        string_free(tex_name);

        /*
         * load frame
         */
        struct xml_element *chars                      = xml_find(xml, "chars", 0);
        struct list_head *head;
        list_for_each(head, &chars->children) {
                struct xml_element *character = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                struct font_frame ff;
                ff.size         = size;
                ff.line_height  = line_height;
                ff.base         = base;
                ff.tex_width    = tex_width;
                ff.tex_height   = tex_height;
                ff.texid        = index;
                ff.x            = atoi(xml_find_attribute(character, "x")->value->ptr);
                ff.y            = atoi(xml_find_attribute(character, "y")->value->ptr);
                ff.width        = atoi(xml_find_attribute(character, "width")->value->ptr);
                ff.height       = atoi(xml_find_attribute(character, "height")->value->ptr);
                ff.xoffset      = atoi(xml_find_attribute(character, "xoffset")->value->ptr);
                ff.yoffset      = atoi(xml_find_attribute(character, "yoffset")->value->ptr);
                ff.xadvance     = atoi(xml_find_attribute(character, "xadvance")->value->ptr);
                struct xml_attribute *id = xml_find_attribute(character, "id");
                u32 n           = atoi(id->value->ptr);
                ff.code         = n;
                map_set(font, &n, sizeof(n), &ff);
        }

        xml_free(xml);
        map_set(content->atlases, font_path, strlen(font_path), &font);
}

void render_content_set_atlas(struct render_content *content, u16 index, char *atlas_path)
{
        struct map *atlas                       = map_alloc(sizeof(struct texture_frame));
        struct xml_element *xml                 = xml_parse(atlas_path);
        struct xml_element *TextureAtlas        = xml_find(xml, "TextureAtlas", 0);
        struct xml_attribute *TexturePath       = xml_find_attribute(TextureAtlas, "imagePath");
        struct string *tex_name                 = string_alloc_chars(atlas_path, strlen(atlas_path));
        int i;
        back_i(i, tex_name->len) {
                if(tex_name->ptr[i] == '/') {
                        tex_name->len -= (tex_name->len - 1 - i);
                        string_cat_string(tex_name, TexturePath->value);
                        break;
                }
        }
        render_content_set_texture(content, index, texture_alloc_file(tex_name->ptr));
        string_free(tex_name);

        int tex_width                           = atoi(xml_find_attribute(TextureAtlas, "width")->value->ptr);
        int tex_height                          = atoi(xml_find_attribute(TextureAtlas, "height")->value->ptr);
        struct list_head *head;
        list_for_each(head, &TextureAtlas->children) {
                struct xml_element *sprite = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                struct xml_attribute *sprite_name       = xml_find_attribute(sprite, "n");
                struct xml_attribute *sprite_x          = xml_find_attribute(sprite, "x");
                struct xml_attribute *sprite_y          = xml_find_attribute(sprite, "y");
                struct xml_attribute *sprite_width      = xml_find_attribute(sprite, "w");
                struct xml_attribute *sprite_height     = xml_find_attribute(sprite, "h");
                struct texture_frame tf;
                tf.x            = atoi(sprite_x->value->ptr);
                tf.y            = atoi(sprite_y->value->ptr);
                tf.width        = atoi(sprite_width->value->ptr);
                tf.height       = atoi(sprite_height->value->ptr);
                tf.tex_width    = tex_width;
                tf.tex_height   = tex_height;
                tf.texid        = index;
                map_set(atlas, sprite_name->value->ptr, sprite_name->value->len, &tf);
        }
        xml_free(xml);
        map_set(content->atlases, atlas_path, strlen(atlas_path), &atlas);
}

struct texture_frame *render_content_get_texture_frame(struct render_content *content, char *atlas, size_t atlas_len, char *key, size_t key_len)
{
        struct texture_frame *tf = NULL;
        struct map *m = map_get(content->atlases, struct map *, atlas, atlas_len);
        if(m) {
                tf = map_get_pointer(m, key, key_len);
        }
        return tf;
}

struct font_frame *render_content_get_font_frame(struct render_content *content, char *font_path, size_t font_path_len, u32 code)
{
        struct font_frame *ff = NULL;
        struct map *m = map_get(content->atlases, struct map *, font_path, font_path_len);
        if(m) {
                ff = map_get_pointer(m, &code, sizeof(code));
        }
        return ff;
}

void render_content_free(struct render_content *content)
{
        i16 i;
        for_i(i, BUFFERS) {
                device_buffer_group_free(content->groups[i]);
        }
        array_deep_free_safe(content->textures, struct texture *, texture_free);
        map_deep_free(content->atlases, struct map *, map_free);

        struct list_head *head;
        list_while_not_singular(head, &content->node_list) {
                struct node *n = (struct node *)
                        ((void *)head - offsetof(struct node, content_head));
                node_free(n);
        }
        /* detach and deallocate */
        list_del_init(&content->queue_head);
        sfree(content);
}

/*
 * node data segment
 */
struct node_data_segment *node_data_segment_alloc()
{
        struct node_data_segment *p = smalloc(sizeof(struct node_data_segment));
        INIT_LIST_HEAD(&p->head);
        p->frames = 0;
        return p;
}

void node_data_segment_free(struct node_data_segment *p)
{
        list_del_init(&p->head);
        sfree(p);
}

/*
 * node data
 */
struct node_data *node_data_alloc()
{
        struct node_data *p = smalloc(sizeof(struct node_data));
        p->buffer_id = 0;
        p->data = bytes_alloc();
        p->fill_segment = node_data_segment_alloc();
        INIT_LIST_HEAD(&p->segments);
        return p;
}

static inline void __node_data_set(struct node_data *p, u8 bid, void *bytes, u32 len)
{
        p->buffer_id = bid;
        if(p->data->ptr != bytes) {
                p->data->len = 0;
                bytes_cat(p->data, bytes, len);
        }

        if(list_singular(&p->fill_segment->head)) {
                list_add_tail(&p->fill_segment->head, &p->segments);
        }
        p->fill_segment->start  = 0;
        p->fill_segment->end    = len - 1;
        p->fill_segment->frames = BUFFERS;
}

static inline void __node_data_set_segment(struct node_data *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if(list_singular(&seg->head)) {
                list_add_tail(&seg->head, &p->segments);
        }
        if((void*)p->data->ptr < bytes || (void*)p->data->ptr > bytes) {
                bytes_sub(p->data, seg->start, bytes, len);
        }
        seg->frames = BUFFERS;
}

void node_data_free(struct node_data *p)
{
        node_data_segment_free(p->fill_segment);
        struct list_head *head;
        list_while_not_singular(head, &p->segments) {
                list_del_init(head);
        }
        bytes_free(p->data);
        sfree(p);
}

struct node *node_alloc(struct render_content *host)
{
        struct node *p          = smalloc(sizeof(struct node));
        p->content_index        = host->current_instances;
        p->host                 = host;

        host->current_instances++;
        list_add_tail(&p->content_head, &host->node_list);

        INIT_LIST_HEAD(&p->updater_head);
        INIT_LIST_HEAD(&p->user_head);

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
        if(list_singular(&d->segments)) {
                array_push(p->pending_datas, &d);
        }
        __node_data_set(d, index, bytes, len);

        if(list_singular(&p->updater_head)) {
                list_add_tail(&p->updater_head, &p->host->pending_updaters);
        }
}

void *node_get_data(struct node *p, u8 index)
{
        struct node_data *d = array_get(p->datas, struct node_data *, index);
        return d->data->ptr;
}

void node_set_data_segment(struct node *p, u8 index, struct node_data_segment *seg, void *bytes, u32 len)
{
        struct node_data *d = array_get(p->datas, struct node_data *, index);
        if(list_singular(&d->segments)) {
                array_push(p->pending_datas, &d);
        }
        __node_data_set_segment(d, seg, bytes, len);
        if(list_singular(&p->updater_head)) {
                list_add_tail(&p->updater_head, &p->host->pending_updaters);
        }
}

static void __node_set_data_self(struct node *p, u8 index, void *bytes, u32 len)
{
        struct node_data *d = array_get(p->datas, struct node_data *, index);
        if(list_singular(&d->segments)) {
                array_push(p->pending_datas, &d);
        } else {
                /* remove all previous segments because we will do a full fill next time */
                struct list_head *head;
                list_while_not_singular(head, &d->segments) {
                        list_del_init(head);
                }
        }
        if(list_singular(&d->fill_segment->head)) {
                __node_data_set(d, index, bytes, len);
        }
        if(list_singular(&p->updater_head)) {
                list_add_tail(&p->updater_head, &p->host->pending_updaters);
        }
}

static inline void __node_submit_data(struct node *p)
{
        struct node_data **data;
        array_for_each(data, p->datas) {
                if((*data)->data->len) {
                        __node_set_data_self(p, (*data)->buffer_id,
                                (*data)->data->ptr, (*data)->data->len);
                }
        }
}

void node_swap(struct node *p1, struct node *p2)
{
        if(p1->host == p2->host) {
                __node_submit_data(p1);
                __node_submit_data(p2);
                u32 i = p1->content_index;
                p1->content_index = p2->content_index;
                p2->content_index = i;
        }
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
                        __node_submit_data(tail);
                        tail->content_index = p->content_index;
                }
                list_del_init(&p->content_head);
                p->host->current_instances--;
        }
        list_del_init(&p->updater_head);
        /* detach from user */
        list_del_init(&p->user_head);

        /* clear data */
        array_free(p->pending_datas);
        array_deep_free(p->datas, struct node_data *, node_data_free);

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
        struct list_head *head;
        /* free stencil queues */
        list_while_not_singular(head, &p->stencil_queue_list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                render_queue_free(queue);
        }
        /* free content queues */
        list_while_not_singular(head, &p->content_queue_list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                render_queue_free(queue);
        }
        /* detach and deallocate */
        list_del_init(&p->renderer_head);
        sfree(p);
}

struct renderer *renderer_alloc()
{
        struct renderer *p = smalloc(sizeof(struct renderer));
        INIT_LIST_HEAD(&p->stage_list);
        INIT_LIST_HEAD(&p->chain_head);
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
        struct list_head *head;
        list_while_not_singular(head, &p->stage_list) {
                struct render_stage *stage = (struct render_stage *)
                        ((void*)head - offsetof(struct render_stage, renderer_head));
                render_stage_free(stage);
        }
        if(p->color) sfree(p->color);
        if(p->pass) p->pass->del(p->pass);
        list_del_init(&p->chain_head);
        sfree(p);
}
