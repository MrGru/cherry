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
#ifndef __CHERRY_GAME_SPRITE_QUAD_TEXT_H__
#define __CHERRY_GAME_SPRITE_QUAD_TEXT_H__

#include <cherry/game/types.h>

struct sprite_quad_text *sprite_quad_text_alloc(struct sprite_quad_text_context *ctx,
        struct sprite_quad_context *content_ctx, float text_size, char *font);

void sprite_quad_text_free(struct sprite_quad_text *p);

void sprite_quad_text_set_text(struct sprite_quad_text *p, char *text, size_t text_len);

void sprite_quad_text_update(struct sprite_quad_text *p);

void sprite_quad_text_set_position(struct sprite_quad_text *p, union vec3 pos);

void sprite_quad_text_set_scale(struct sprite_quad_text *p, union vec3 scale);

void sprite_quad_text_set_rotation(struct sprite_quad_text *p, float rotation);

void sprite_quad_text_set_color(struct sprite_quad_text *p, union vec4 color);

#endif
