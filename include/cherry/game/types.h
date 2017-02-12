#ifndef __CHERRY_GAME_TYPES_H__
#define __CHERRY_GAME_TYPES_H__

#include <cherry/graphic/types.h>

struct game {
        struct list_head        renderer_list;
        struct list_head        node_tree_list;
        struct camera           *cam;
        u8              frame;
};

#endif
