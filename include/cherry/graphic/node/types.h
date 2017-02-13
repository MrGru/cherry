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

struct branch_transform {
        struct list_head        tree_head;

        struct list_head        branch_list;
        struct list_head        branch_head;

        union vec3              position;
        union vec3              scale;
        union vec4              quat;
        union vec3              size;

        union mat4              last_transform;

        u8                      bid;
        u16                     offset_to_node;

        u8                      update;
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

#endif
