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
#ifndef __CHERRY_GRAPHIC_NODE_TYPES_H__
#define __CHERRY_GRAPHIC_NODE_TYPES_H__

#include <cherry/graphic/types.h>

struct branch_z {
        struct list_head        tree_head;

        struct list_head        branch_list;
        struct list_head        branch_head;

        float                   z;

        u8                      bid;
        u16                     offset_to_node;
};

struct branch_transform_queue {
        struct list_head        list;
        struct branch_transform *root;
        u8                      full;
};

struct rotation_vector {
        struct list_head        head;
        struct list_head        action;
        union vec4              rad_vec3;
};

/*
 * branch_transform_queue only keep list of branchs knowing
 * which sub branchs would be updated by child_updater_list;
 * because there may be many branchs be updating have same parent
 * so branch_transform_queue.list tends to be small; when a branch
 * is shaked it will remove all sub branchs from branch_transform_queue.list,
 * try to append it's parent to list if no super parent is updated.
 * so instead of traverse from root to update transform tree, we only
 * need to traverse branch_transform_queue.list and we know exactly the
 * branchs are shaked.
 * I choose this approach because I think transform tree will have low depth
 * and branch_transform_queue.list is small.
 *
 * @updater_head only belong to child_updater_list and we traverse from
 * child_updater_list, not do anything more with updater_head so when a branch
 * need remove all sub branchs be updating, we only need reinit their sub branch's
 * child_updater_list; updater_head may keep pointer to parent's update list although
 * parent is not in branch_transform_queue.list
 */
struct branch_transform {
        struct list_head                tree_head;

        struct list_head                branch_list;
        struct list_head                branch_head;
        struct branch_transform         *parent;

        struct list_head                child_updater_list;
        struct list_head                updater_head;

        struct list_head                update_queue_head;
        struct branch_transform_queue   *update_queue;

        /*
         * I need position, scale, size to be in uniform size with quat (vec4)
         * due to operating in action sequence
         */
        union {
                union vec3              position;
                union vec4              position_expanded;
        };
        union {
                union vec3              scale;
                union vec4              scale_expanded;
        };
        union {
                union vec3              size;
                union vec4              size_expanded;
        };
        union vec4                      quat;

        struct list_head                anim_rotations;

        union mat4                      last_transform;

        u8                              bid;
        u16                             offset_to_node;

        u8                              update;
};

struct branch_color {
        struct list_head        tree_head;

        struct list_head        branch_list;
        struct list_head        branch_head;

        union vec4              color;
        union vec4              display_color;
        u8                      pass;

        u8                      bid;
        u16                     offset_to_node;
};

struct twig_texid {
        struct list_head        tree_head;

        float                   texid;

        u8                      bid;
        u16                     offset_to_node;
};

struct twig_texcoord {
        struct list_head        tree_head;

        union vec4              texcoord[3];

        u8                      bid[3];
        u16                     offset_to_node;
};

struct twig_vertex {
        struct list_head        tree_head;

        union vec4              vertex[3];

        u8                      bid[3];
        u16                     offset_to_node;
};

struct twig_vertex_color {
        struct list_head        tree_head;

        u8                      bid;
        u16                     offset_to_node;
};

/*
 * node_tree is useful to render 2d graph
 *
 * it can maintain a tree having different textures
 * but consume only one drawcall
 */
struct node_tree {
        struct list_head        node_head;

        struct list_head        z;
        struct list_head        transform;
        struct list_head        color;
        struct list_head        texid;
        struct list_head        texcoord;
        struct list_head        vertex;

        struct list_head        life_head;
};

/*
 * node 3d color
 */
struct twig_3d_vertex {
        struct list_head        tree_head;

        u8                      bid[3];
        u16                     offset_to_node;
};

struct twig_3d_normal {
        struct list_head        tree_head;

        u8                      bid[3];
        u16                     offset_to_node;
};

struct node_3d_color {
        struct list_head        node_head;

        struct list_head        transform;
        struct list_head        color;
        struct list_head        vertex;
        struct list_head        normal;
        struct list_head        vertex_color;

        struct list_head        life_head;
};

/*
 * node action
 */
enum {
        EASE_LINEAR,
        EASE_QUADRATIC_IN,
        EASE_QUADRATIC_OUT,
        EASE_QUADRATIC_IN_OUT,
        EASE_CUBIC_IN,
        EASE_CUBIC_OUT,
        EASE_CUBIC_IN_OUT,
        EASE_QUARTIC_IN,
        EASE_QUARTIC_OUT,
        EASE_QUARTIC_IN_OUT,
        EASE_QUINTIC_IN,
        EASE_QUINTIC_OUT,
        EASE_QUINTIC_IN_OUT,
        EASE_SINUSOIDAL_IN,
        EASE_SINUSOIDAL_OUT,
        EASE_SINUSOIDAL_IN_OUT,
        EASE_EXPONENTIAL_IN,
        EASE_EXPONENTIAL_OUT,
        EASE_EXPONENTIAL_IN_OUT,
        EASE_CIRCULAR_IN,
        EASE_CIRCULAR_OUT,
        EASE_CIRCULAR_IN_OUT,

        EASE_GRAVITY,
        EASE_SEQUENCE,
        EASE_PARALLEL,

        EASE_FORCE
};

struct action {
        struct list_head                user_head;
        struct list_head                head;
        struct list_head                children;

        u8                              ease_type;

        union vec4                      *target;
        union {
                struct {
                        union vec4      offset;
                        union vec4      last_amount_offset;

                        float           duration;
                        float           remain;
                };
                struct {
                        float           velocity;
                        float           accelerate;

                        struct array    *destinations;

                        i16             index;
                };
                struct {
                        union vec4      destination;
                };
        };

        u8                              finish;

        i16                             repeat;
};

struct action_key {
        struct list_head                key_head;
        struct list_head                actions;
        struct branch_transform         *transform;
};

struct action_manager {
        struct list_head                keys;
};

#endif
