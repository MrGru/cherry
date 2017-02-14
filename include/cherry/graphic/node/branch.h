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
#ifndef __CHERRY_GRAPHIC_NODE_BRANCH_H__
#define __CHERRY_GRAPHIC_NODE_BRANCH_H__

#include <cherry/graphic/node/types.h>

struct branch_transform_queue *branch_transform_queue_alloc();

void branch_transform_queue_traverse(struct branch_transform_queue *p);

void branch_transform_queue_free(struct branch_transform_queue *p);

/*
 * branch z definitions
 */
struct branch_z *branch_z_alloc(u8 bid);

void branch_z_free(struct branch_z *p);

void branch_z_add(struct branch_z *parent, struct branch_z *child);

void branch_z_del(struct branch_z *p);

void branch_z_traverse(struct branch_z *p, float *cz);

/*
 * branch transform definitions
 */
struct branch_transform *branch_transform_alloc(u8 bid, struct branch_transform_queue *queue);

void branch_transform_free(struct branch_transform *p);

void branch_transform_add(struct branch_transform *parent, struct branch_transform *child);

void branch_transform_del(struct branch_transform *p);

void branch_transform_shake(struct branch_transform *p);

void branch_transform_traverse(struct branch_transform *p, union mat4 cm);

/*
 * branch color definitions
 */
struct branch_color *branch_color_alloc(u8 bid);

void branch_color_free(struct branch_color *p);

void branch_color_add(struct branch_color *parent, struct branch_color *child);

void branch_color_del(struct branch_color *p);

void branch_color_traverse(struct branch_color *p, union vec4 cc);

#endif
