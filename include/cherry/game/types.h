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

/*
 * game element
 */
enum {
        GEM_1_LV_1,
        GEM_2_LV_1,
        GEM_3_LV_1,
        GEM_4_LV_1,
        GEM_5_LV_1,
        GEM_6_LV_1,
        GEM_1_LV_2,
        GEM_2_LV_2,
        GEM_3_LV_2,
        GEM_4_LV_2,
        GEM_5_LV_2,
        GEM_6_LV_2,
        GEM_1_LV_3,
        GEM_2_LV_3,
        GEM_3_LV_3,
        GEM_4_LV_3,
        GEM_5_LV_3,
        GEM_6_LV_3
};

#define element_is_gem(elm) ((elm)->type >= GEM_1_LV_1 && (elm)->type <= GEM_6_LV_3)

struct game_element {
        struct list_head                path_head;

        u16                             type;
};

struct gem {
        struct game_element             elm;

        struct node_3d_color            *node;

        struct node_3d_color            *flipped_node;

        struct action_key               node_move_key;
        struct action_key               node_choice_key;
        struct action_key               node_unchoice_key;
        struct action_key               node_collected_key;
};

/*
 * game connect point
 * neighbor :
 *      0 1 2
 *      7   3
 *      6 5 4
 */
struct path_point;
struct element_deliver;

struct path_point {
        struct list_head                neighbor[8];
        struct list_head                neighbor_head[8];

        struct list_head                dynamic_element;
        struct list_head                obstacle_lv1;
        struct list_head                obstacle_lv2;

        union {
                union vec3              position;
                union vec4              position_expanded;
        };

        struct array                    *delivers;
};

enum {
        ELEMENT_DELIVER_PATH_POINT,
        ELEMENT_DELIVER_POOL,
        ELEMENT_DELIVER_GATE
};

/*
 * @trace_head : used to maintain moving algorithm
 */
struct element_deliver {
        struct list_head                                trace_head;
        u8                                              type;
        union {
                /*
                 * ppoint active when type is ELEMENT_DELIVER_PATH_POINT, ELEMENT_DELIVER_GATE
                 */
                struct {
                        struct path_point               *ppoint;
                        union vec4                      gate_pos_expanded;
                };
                /*
                 * element pool active when type is ELEMENT_DELIVER_POOL
                 */
                struct {
                        struct list_head                *element_list;
                        union {
                                union vec3              position;
                                union vec4              position_expanded;
                        };
                };
        };
};

/*
 * game context
 */
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

        struct action_manager           *action_manager;

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
