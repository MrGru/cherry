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
#ifndef __CHERRY_GRAPHIC_NODE_NODE_TREE_H__
#define __CHERRY_GRAPHIC_NODE_NODE_TREE_H__

#include <cherry/graphic/node/types.h>

/*
 * node_tree allocations
 */
struct node_tree *node_tree_alloc(struct node *n);

void node_tree_free(struct node_tree *p);

void node_tree_set_node(struct node_tree *p, struct node *n);

void node_tree_add_node_tree(struct node_tree *p, struct node_tree *q);

/*
 * node_tree setup branchs and twigs
 */
void node_tree_set_branch_z(struct node_tree *p, struct branch_z *b);

void node_tree_set_branch_transform(struct node_tree *p, struct branch_transform *b);

void node_tree_set_branch_color(struct node_tree *p, struct branch_color *b);

void node_tree_set_twig_texid(struct node_tree *p, struct twig_texid *b);

void node_tree_set_twig_texcoord(struct node_tree *p, struct twig_texcoord *b);

void node_tree_set_twig_vertex(struct node_tree *p, struct twig_vertex *b);

struct branch_z *node_tree_get_branch_z(struct node_tree *p);

struct branch_transform *node_tree_get_branch_transform(struct node_tree *p);

struct branch_color *node_tree_get_branch_color(struct node_tree *p);

struct node *node_tree_get_node(struct node_tree *p);

/*
 * node_tree manipulate datas
 */
void node_tree_set_position(struct node_tree *p, union vec3 v);

void node_tree_set_scale(struct node_tree *p, union vec3 v);

void node_tree_set_rotation(struct node_tree *p, union vec4 quat);

void node_tree_set_size(struct node_tree *p, union vec3 size);

void node_tree_set_texcoord(struct node_tree *p, u8 id, union vec2 coord, u8 update);

void node_tree_set_vertex(struct node_tree *p, u8 id, union vec2 vertex, u8 update);

void node_tree_set_texid(struct node_tree *p, u8 id);

void node_tree_set_color(struct node_tree *p, union vec4 color);

union vec3 *node_tree_get_position(struct node_tree *p);

union vec3 *node_tree_get_scale(struct node_tree *p);

union vec4 *node_tree_get_rotation(struct node_tree *p);

#endif
