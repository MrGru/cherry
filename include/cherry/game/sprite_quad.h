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

void sprite_quad_set_texture_frame(struct sprite_quad *p, struct texture_frame *tf);

void sprite_quad_set_texture(struct sprite_quad *p, int texid);

void sprite_quad_set_position(struct sprite_quad *p, union vec3 pos);

void sprite_quad_set_scale(struct sprite_quad *p, union vec3 scale);

void sprite_quad_set_rotation(struct sprite_quad *p, float rotation);

void sprite_quad_update(struct sprite_quad *p);

#endif
