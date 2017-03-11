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
#ifndef __CHERRY_GRAPHIC_NODE_NODE_3D_COLOR_H__
#define __CHERRY_GRAPHIC_NODE_NODE_3D_COLOR_H__

#include <cherry/graphic/node/types.h>

struct node_3d_color *node_3d_color_alloc(struct node *n);

void node_3d_color_free(struct node_3d_color *p);

void node_3d_color_add_node_3d_color(struct node_3d_color *p, struct node_3d_color *q);

/*
 * node_3d_color setup branchs and twigs
 */
void node_3d_color_set_branch_transform(struct node_3d_color *p, struct branch_transform *b);

void node_3d_color_set_branch_color(struct node_3d_color *p, struct branch_color *b);

void node_3d_color_set_twig_3d_vertex(struct node_3d_color *p, struct twig_3d_vertex *b);

void node_3d_color_set_twig_3d_normal(struct node_3d_color *p, struct twig_3d_normal *b);

void node_3d_color_set_twig_vertex_color(struct node_3d_color *p, struct twig_vertex_color *b);

void node_3d_color_set_twig_bright(struct node_3d_color *p, struct twig_bright *b);

struct branch_transform *node_3d_color_get_branch_transform(struct node_3d_color *p);

struct branch_color *node_3d_color_get_branch_color(struct node_3d_color *p);

struct twig_bright *node_3d_color_get_twig_bright(struct node_3d_color *p);

struct twig_3d_vertex *node_3d_color_get_twig_3d_vertex(struct node_3d_color *p);

struct node *node_3d_color_get_node(struct node_3d_color *p);

/*
 * node_3d_color manipulate datas
 */
void node_3d_color_set_position(struct node_3d_color *p, union vec3 v);

void node_3d_color_set_scale(struct node_3d_color *p, union vec3 v);

void node_3d_color_set_rotation(struct node_3d_color *p, union vec4 quat);

void node_3d_color_set_size(struct node_3d_color *p, union vec3 size);

void node_3d_color_fill_vertex(struct node_3d_color *p, void *v1, void *v2, void *v3, u32 len);
void node_3d_color_set_vertex_1(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);
void node_3d_color_set_vertex_2(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);
void node_3d_color_set_vertex_3(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);

void node_3d_color_fill_normal(struct node_3d_color *p, void *v1, void *v2, void *v3, u32 len);
void node_3d_color_set_normal_1(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);
void node_3d_color_set_normal_2(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);
void node_3d_color_set_normal_3(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);

void node_3d_color_fill_vertex_color(struct node_3d_color *p, void *v1, u32 len);
void node_3d_color_set_vertex_color(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len);

void node_3d_color_set_color(struct node_3d_color *p, union vec4 color);

void node_3d_color_set_bright(struct node_3d_color *p, float bright);

union vec3 *node_3d_color_get_position(struct node_3d_color *p);

union vec3 *node_3d_color_get_scale(struct node_3d_color *p);

union vec4 *node_3d_color_get_rotation(struct node_3d_color *p);

#endif
