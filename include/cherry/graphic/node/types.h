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

        u8                      bid;
        u16                     offset_to_node;

        u8                      update;
};

struct node_tree {
        struct list_head                node_head;

        struct list_head                z;
        struct list_head                transform;
};

#endif
