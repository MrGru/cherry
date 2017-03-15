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
#ifndef __CHERRY_GAME_SPRITE_QUAD_H__
#define __CHERRY_GAME_SPRITE_QUAD_H__

#include <cherry/game/types.h>

struct sprite_quad *sprite_quad_alloc();

void sprite_quad_free(struct sprite_quad *p);

#endif
