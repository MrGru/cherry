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
#include <cherry/graphic/node/types.h>
#include <cherry/graphic/light/types.h>

struct game {
        struct list_head                renderer_list;
        struct list_head                node_tree_list;
        struct list_head                node_3d_color_list;

        struct branch_transform_queue   *update_queue;
        struct branch_transform_queue   *n3d_update_queue;

        struct render_content           *ui_content;
        struct render_content           *game_content;

        struct camera                   *ui_cam;
        struct camera                   *game_cam;

        struct shader_uniform           *ui_projection_uniform;
        struct shader_uniform           *game_projection_uniform;

        struct node_tree                *ui_root;
        struct node_3d_color            *n3d_color_root;

        struct point_light              *world_light;

        u8                              frame;
};

struct ui_sprite_param {
        u8              texid;
        union vec3      size;
        union vec2      texcoord[6];
};

struct n3d_color_param {
        union vec3      size;
        union vec3      *v1;
        union vec3      *v2;
        union vec3      *v3;
        union vec3      *n1;
        union vec3      *n2;
        union vec3      *n3;
        u32             vlen;
};

#endif
