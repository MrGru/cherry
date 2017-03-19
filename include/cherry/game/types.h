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
#include <cherry/graphic/dae/types.h>

/*
 * a model having large number of vertex can separate into several node
 * or create a new render_content for it own
 */
#define GAME_TRIANGLES_PER_OBJECT       320

#define CELL_SIZE                       200

/*
 * game element
 */
enum {
        GEM_RED,
        GEM_BLUE,
        GEM_GREEN,
        GEM_PURPLE,
        GEM_ORANGE,

        GEM_BLUE_HORIZON,
        GEM_RED_HORIZON,
        GEM_GREEN_HORIZON,
        GEM_PURPLE_HORIZON,
        GEM_ORANGE_HORIZON,

        GEM_RED_VERTICAL,
        GEM_BLUE_VERTICAL,
        GEM_GREEN_VERTICAL,
        GEM_PURPLE_VERTICAL,
        GEM_ORANGE_VERTICAL
};

#define element_is_gem(elm) ((elm)->type >= GEM_RED && (elm)->type <= GEM_ORANGE_VERTICAL)

struct game_element {
        struct list_head                life_head;
        struct list_head                path_head;
        struct list_head                update_pos_head;

        u16                             type;
};

struct gem {
        struct game_element             elm;
        struct node_3d_color            *node;

        struct node_3d_color            *flipped_node;

        struct action_key               node_move_key;

        struct action                   *sequence;
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
        struct list_head                touch_head;
        struct list_head                life_head;
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
        struct list_head                                life_head;
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
                        float                           delay;
                        struct list_head                reduce_delay_head;
                };
        };
};

struct connect_line {
        struct list_head                                head;
        union mat4                                      transform;
        union {
                struct {
                        struct node_data_segment        *seg_v1;
                        struct node_data_segment        *seg_v2;
                        struct node_data_segment        *seg_v3;
                };
                struct node_data_segment                *seg[3];
        };
        struct node_3d_color                            *node;
};

struct effect_star {
        struct list_head                                head;
        union mat4                                      transform;
        union {
                union vec3                              position;
                union vec4                              position_expanded;
        };
        union vec4                                      rotate;
        union {
                union vec3                              scale;
                union vec4                              scale_expanded;
        };
        union {
                struct {
                        struct node_data_segment        *seg_v1;
                        struct node_data_segment        *seg_v2;
                        struct node_data_segment        *seg_v3;
                };
                struct node_data_segment                *seg[3];
        };
        struct node_3d_color                            *node;
        struct action_key                               move_key;
};

struct sprite_quad_context {
        struct list_head        free_list;
        struct list_head        use_list;
        struct list_head        update_list;
};

struct sprite_quad {
        struct list_head                                head;
        struct list_head                                update_head;

        union mat4                                      super_transform;
        union mat4                                      last_transform;
        union mat4                                      transform;
        union {
                union vec3                              position;
                union vec4                              position_expanded;
        };
        union vec4                                      rotate;
        union {
                union vec3                              scale;
                union vec4                              scale_expanded;
        };
        union vec4                                      color;
        union {
                struct {
                        struct node_data_segment        *seg_v1;
                        struct node_data_segment        *seg_v2;
                        struct node_data_segment        *seg_v3;
                        struct node_data_segment        *seg_texcoord;
                        struct node_data_segment        *seg_color;
                };
                struct node_data_segment                *seg[5];
        };
        struct node_3d_color                            *node;
        struct texture_frame                            *current_frame;
        struct action_key                               key;
        struct sprite_quad_context                      *context;
};

struct sprite_quad_text_context {
        struct list_head        use_list;
        struct list_head        update_list;
};

enum {
        TEXT_ALIGN_LEFT,
        TEXT_ALIGN_RIGHT,
        TEXT_ALIGN_BALANCE
};

struct sprite_quad_text {
        struct list_head                        head;
        struct list_head                        update_head;

        struct array                            *quads;
        struct sprite_quad_context              *content_context;
        struct sprite_quad_text_context         *context;
        struct string                           *text;
        struct string                           *font;
        float                                   text_size;
        int                                     text_align;

        union {
                struct {
                        float                   width;
                        float                   height;
                };
                union vec2                      size;
        };


        union {
                union vec3                              position;
                union vec4                              position_expanded;
        };
        union vec4                                      rotate;
        union {
                union vec3                              scale;
                union vec4                              scale_expanded;
        };
        union vec4                                      color;

        struct action_key                               key;
};

enum {
        PLAY_IDLE,
        PLAY_SEARCH_NODE,
        PLAY_PROCESS
};

/*
 * game context
 */
struct game {
        struct list_head                renderer_list;
        struct list_head                node_3d_color_list;
        struct list_head                element_list;
        struct list_head                element_pool_list;
        struct list_head                deliver_list;
        struct list_head                deliver_delay_list;
        struct list_head                path_point_list;
        struct list_head                element_update_pos_list;
        /*
         * connect lines list
         */
        struct list_head                free_connect_line_list;
        struct list_head                using_connect_line_list;
        /*
         * effect stars list
         */
        struct list_head                free_effect_star_list;
        struct list_head                using_effect_star_list;

        /*
         * background effect
         */
        union {
                struct {
                        struct sprite_quad_context      low_background_context;
                        struct sprite_quad_context      middle_background_context;
                        struct sprite_quad_context      top_background_context;
                };
                struct sprite_quad_context      sprite_quad_context_list[3];
        } __attribute__((packed));

        struct list_head                touching_gem_list;

        struct sprite_quad_text_context text_context;

        u8                              connecting_gems_number;

        u8                              play_state;

        float                           check_path_delay;


        struct path_point               *path_point_touches[9][9];

        struct branch_transform_queue   *n3d_update_queue;

        struct render_content           *game_content;

        struct camera                   *game_cam;

        struct shader_uniform           *game_projection_uniform;

        struct direction_light          *world_direction_light;

        struct action_manager           *action_manager;

        i16                             can_draw;

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
        union vec4      *color;
        union texcoord_3d      *texcoord;
        u32             vlen;
        u32             clen;
        u32             tlen;
};

#endif
