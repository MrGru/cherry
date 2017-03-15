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
#ifndef __CHERRY_GAME_EFFECT_STAR_H__
#define __CHERRY_GAME_EFFECT_STAR_H__

#include <cherry/game/types.h>

struct effect_star *effect_star_alloc();

void effect_star_free(struct effect_star *p);

void effect_star_show(struct effect_star *p, union vec3 pos, struct game *g);

void effect_star_update(struct effect_star *p, struct game *g);

#endif
