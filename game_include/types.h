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
#ifndef __CHERRY_GAME_TYPES_H__
#define __CHERRY_GAME_TYPES_H__

#include <cherry/graphic/types.h>

struct game {
        union {
                struct {
                        struct node_manager     *manager_game;
                        struct node_manager     *manager_hud;
                };
                struct node_manager             *manager[2];
        };
        union {
                struct {
                        struct camera           *game_camera;
                        struct camera           *hud_camera;
                };
                struct camera                   *camera[2];
        };

        u8                                      frame;
};

#endif
