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

        union vec3                      position;
        union vec3                      scale;
        union vec4                      quat;
        union vec3                      size;

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

/*
 * node_tree is useful to render 2d graph
 *
 * it can maintain a tree having different textures
 * but consume only one drawcall
 */
struct node_tree {
        struct list_head                node_head;

        struct list_head                z;
        struct list_head                transform;
        struct list_head                color;
        struct list_head                texid;
        struct list_head                texcoord;
        struct list_head                vertex;

        struct list_head                life_head;
};

struct node_single {
        struct list_head                node_head;

        struct list_head                life_head;
};

#endif
