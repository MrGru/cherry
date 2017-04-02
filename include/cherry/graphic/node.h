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
#ifndef __CHERRY_GRAPHIC_NODE_H__
#define __CHERRY_GRAPHIC_NODE_H__

#include <cherry/graphic/types.h>

/*
 * node render declarations
 */
struct node_render_buffer_group *node_render_buffer_group_alloc();
void node_render_buffer_group_free(struct node_render_buffer_group *p);
void node_render_buffer_group_clear_group(struct node_render_buffer_group *p);
void node_render_buffer_group_clear_group_keep(struct node_render_buffer_group *p);
void node_render_buffer_group_clear_texture(struct node_render_buffer_group *p);
void node_render_buffer_group_add_texture(struct node_render_buffer_group *p, struct texture *tex);

struct node_render_content *node_render_content_alloc();
void node_render_content_clear(struct node_render_content *p);
void node_render_content_clear_keep(struct node_render_content *p);
void node_render_content_reserve(struct node_render_content *p, int count);
void node_render_content_free(struct node_render_content *p);

/*
 * node declarations
 */
struct node *node_alloc(struct node_manager *m);

void node_free(struct node *p);

void node_show_sprite(struct node *p, u32 shader_type, struct texture *tex);

void node_show_spine(struct node *p, u32 shader_type, char *spine_file, char *atlas_file, float scale);

void node_update_spine(struct node *p, float time, u8 frame);

void node_spine_set_animation(struct node * p, int trackindex, char *name, int loop);
void node_spine_add_animation(struct node * p, int trackindex, char *name, int loop, float delay);
void node_spine_run_animation(struct node *p);
void node_spine_stop_animation(struct node *p);

void node_request_update_transform(struct node *p);

void node_set_position(struct node *p, union vec3 position);

void node_set_scale(struct node *p, union vec3 scale);

void node_set_size(struct node *p, union vec3 size);

void node_set_rotation(struct node *p, union vec4 quaternion);

void node_set_origin(struct node *p, union vec3 origin);

void node_set_visible(struct node *p, u8 visible);

void node_add_child(struct node *p, struct node *child);

void node_update_transform(struct node *p);

#if GFX == OGL
void node_setup_buffer(struct device_buffer *buffer, struct shader *s, u32 key);
#endif

int node_setup_common_uniform(struct node *p);
int node_setup_uniform(struct node *p);

void node_render(struct node *p, u8 frame);

#endif
