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

struct element_type {
        union vec4      color;
};

extern struct element_type         __gem_1__;
extern struct element_type         __gem_2__;
extern struct element_type         __gem_3__;
extern struct element_type         __gem_4__;
extern struct element_type         __gem_5__;
extern struct element_type         __gem_6__;
/*
 * expand custom element type here
 */

/*
 * game element
 */
struct element {
        struct list_head        life_head;
        struct list_head        game_head;
        struct list_head        pend_head;

        struct list_head        path_head;

        struct node_3d_color    *main;
        struct element_type     *type;
};

/*
 * game connect point
 */
struct path_point {
        struct list_head        next_list;
        struct list_head        next_head_left;
        struct list_head        next_head_center;
        struct list_head        next_head_right;

        struct list_head        element;

        union vec3              position;
};

/*
 * game connect point including references to 8 neighbors around
 */
struct game_point {
        struct path_point       point;

        struct list_head        neighbor[8];
        struct list_head        neighbor_head[8];
};

struct game {
        struct list_head                renderer_list;
        struct list_head                node_tree_list;
        struct list_head                node_3d_color_list;
        struct list_head                all_gem_list;
        struct list_head                game_gem_list;
        struct list_head                pend_gem_list;

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
        union vec3      *color;
        u32             vlen;
};

#endif
