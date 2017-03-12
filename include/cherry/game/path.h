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
#ifndef __CHERRY_GAME_PATH_H__
#define __CHERRY_GAME_PATH_H__

#include <cherry/game/types.h>

struct path_point *path_point_alloc();

void path_point_free(struct path_point *p);

void path_point_add_neighbor(struct path_point *p, struct path_point *n, u8 index);

void path_point_check_move(struct path_point *p, struct game *game);

int path_point_is_neighbor(struct path_point *p, struct path_point *q);

#endif
