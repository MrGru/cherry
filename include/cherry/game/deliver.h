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
#ifndef __CHERRY_GAME_DELIVER_H__
#define __CHERRY_GAME_DELIVER_H__

#include <cherry/game/types.h>

struct element_deliver *element_deliver_alloc_path_point(struct path_point *pp);

struct element_deliver *element_deliver_alloc_gate(struct path_point *pp);

struct element_deliver *element_deliver_alloc_gem_list(struct list_head *list, union vec4 pos);

void element_deliver_reduce_delay(struct element_deliver *p, float t);

void element_deliver_free(struct element_deliver *p);

#endif
