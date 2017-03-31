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
#ifndef __CHERRY_GAME_GAME_H__
#define __CHERRY_GAME_GAME_H__

#include <types.h>

struct game *game_alloc();

void game_update(struct game *p);

void game_render(struct game *p);

void game_read_event(struct game *p, struct event *e);

void game_free(struct game *p);

void game_resize(struct game *p, int width, int height);

#endif
