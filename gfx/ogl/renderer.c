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

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/graphic/texture.h>

static u8 depth_testing = 0;

static inline void begin_stencil()
{
        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT);
        glClearStencil(0);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glColorMask(0, 0, 0, 0);
}

static inline void begin_stencil_content()
{
        glColorMask(255, 255, 255, 255);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);
}

static inline void end_stencil()
{
        glDisable(GL_STENCIL_TEST);
}

static inline void queue_render(struct render_queue *queue, u8 frame)
{
        /* apply pipeline */
        shader_use(queue->pipeline);
        /* draw */
        struct list_head *head;
        list_for_each(head, &queue->content_list) {
                struct render_content *content = (struct render_content *)
                        ((void*)head - offsetof(struct render_content, queue_head));
                /* bind textures */
                struct texture **tex;
                i16 i;
                array_for_each_index(tex, i, content->textures) {
                        if(*tex) {
                                texture_bind(*tex);
                                GLint active_id = (GLint)(*tex)->active_id;
                                struct shader_uniform *uniform = array_get(queue->pipeline->texture_uniforms,
                                        struct shader_uniform *, i);
                                shader_uniform_update(uniform, &active_id, sizeof(active_id));
                        }
                }
                /* push uniform datas to device memory */
                shader_update_uniform(queue->pipeline, frame);
                /* update buffers */
                struct list_head *updater, *next_updater;
                list_for_each_safe(updater, next_updater, &content->pending_updaters) {
                        struct node *node = (struct node *)
                                ((void*)updater - offsetof(struct node, updater_head));
                        struct node_data **data;
                        i16 data_index;
                        array_for_each_index(data, data_index, node->pending_datas) {
                                // (*data)->frames--;
                                struct device_buffer *buffer = array_get(content->groups[frame]->buffers,
                                        struct device_buffer *, (*data)->buffer_id);
                                struct list_head *segment_head, *segment_head_next;
                                list_for_each_safe(segment_head, segment_head_next, &(*data)->segments) {
                                        struct node_data_segment *seg = (struct node_data_segment *)
                                                ((void *)segment_head - offsetof(struct node_data_segment, head));
                                        seg->frames--;
                                        device_buffer_sub(buffer, node->content_index * (*data)->data->len + seg->start,
                                                (*data)->data->ptr + seg->start, seg->end - seg->start + 1);
                                        if(seg->frames == 0) {
                                                list_del_init(segment_head);
                                        }
                                }
                                if(list_singular(&(*data)->segments)) {
                                        array_remove(node->pending_datas, data_index);
                                        data_index--;
                                        data--;
                                }
                                // device_buffer_sub(buffer, node->content_index * (*data)->data->len,
                                //         (*data)->data->ptr, (*data)->data->len);
                                // if((*data)->frames == 0) {
                                //         array_remove(node->pending_datas, data_index);
                                //         data_index--;
                                //         data--;
                                // }
                        }
                        if(node->pending_datas->len == 0) list_del_init(updater);
                }
                /* bind vao and draw */
                if(content->current_instances) {
                        if(content->depth_test) {
                                if(!depth_testing) {
                                        glEnable(GL_DEPTH_TEST);
                                        depth_testing = 1;
                                }
                        } else {
                                if(depth_testing) {
                                        glDisable(GL_DEPTH_TEST);
                                        depth_testing = 0;
                                }
                        }
                        device_buffer_group_bind_draw(content->groups[frame]);
                        glDrawArraysInstanced(GL_TRIANGLES, 0, content->vertice,
                                content->current_instances * content->instance_multiple);
                }
        }
}

static inline void queue_list_render(struct list_head *list, u8 frame)
{
        struct list_head *head, *next;
        list_for_each_safe(head, next, list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                queue_render(queue, frame);
        }
}

static struct render_pass *current_pass = NULL;

void renderer_render(struct renderer *p, u8 frame)
{
        if(current_pass != p->pass) {
                glBindFramebuffer(GL_FRAMEBUFFER, p->pass->id);
                current_pass = p->pass;
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        if(p->color) {
                glClearColor(devec4(*p->color));
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
                glClear(GL_DEPTH_BUFFER_BIT);
        }

        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->stage_list) {
                struct render_stage *stage = (struct render_stage *)
                        ((void*)head - offsetof(struct render_stage, renderer_head));
                /* enable and render clipping stencil */
                if(!list_singular(&stage->stencil_queue_list)) {
                        begin_stencil();
                        queue_list_render(&stage->stencil_queue_list, frame);
                        begin_stencil_content();
                }
                /* render main content */
                queue_list_render(&stage->content_queue_list, frame);
                /* disable stencil */
                if(!list_singular(&stage->stencil_queue_list)) {
                        end_stencil();
                }
        }
}

#endif
