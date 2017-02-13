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
#ifndef __CHERRY_GRAPHIC_RENDER_H__
#define __CHERRY_GRAPHIC_RENDER_H__

#include <cherry/graphic/types.h>

/*
 * allocate new render_queue
 * @stage       : render_stage that queue attachs to
 * @pipeline    : program used to render content list
 */
struct render_queue *render_queue_alloc(struct list_head *stage, struct shader *pipeline);

void render_queue_free(struct render_queue *queue);

/*
 * allocate new render_content
 * content groups are built from queue's pipeline and mesh's buffers
 */
struct render_content *render_content_alloc(struct render_queue *queue,
        struct array *buffers[BUFFERS], u16 vertice, u16 max_instances);

void render_content_set_texture(struct render_content *content, u16 index, struct texture *t);

void render_content_free(struct render_content *content);

struct node_data *node_data_alloc();
void node_data_set(struct node_data *p, u8 bid, void *bytes, u32 len);
void node_data_free(struct node_data *p);

/*
 * allocate new node from host
 */
struct node *node_alloc(struct render_content *host);

/*
 * change node instance data of index-th buffer in host
 */
void node_set_data(struct node *p, u8 index, void *bytes, u32 len);

/*
 * detach p and all it's children from host, current tree and deallocate it
 */
void node_free(struct node *p);

/*
 * allocate new render_stage
 * @renderer    : renderer that stage attachs to
 */
struct render_stage *render_stage_alloc(struct renderer *renderer);

void render_stage_free(struct render_stage *p);

/*
 * render pass allocations
 */
struct render_pass *render_pass_main_alloc();

struct render_pass *render_pass_shadow_alloc();

/*
 * allocate new renderer
 */
struct renderer *renderer_alloc();

/*
 * render frame
 */
void renderer_render(struct renderer *p, u8 frame);

void renderer_set_color(struct renderer *p, union vec4 *color);

/*
 * deallocate renderer and content
 */
void renderer_free(struct renderer *p);

#endif
