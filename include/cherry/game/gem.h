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
#ifndef __CHERRY_GAME_GEM_H__
#define __CHERRY_GAME_GEM_H__

#include <cherry/game/types.h>

struct gem *gem_alloc(u16 type);

void gem_free(struct gem *p);

void gem_set_node(struct gem *p, struct node_3d_color *node, struct node_3d_color *flipped_node);

void gem_update_node(struct gem *p);

void gem_reset_mesh(struct gem *p);

struct dae_mesh *gem_mesh_cache(u16 type);

#endif
