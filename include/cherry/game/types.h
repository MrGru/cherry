#ifndef __CHERRY_GAME_TYPES_H__
#define __CHERRY_GAME_TYPES_H__

#include <cherry/graphic/types.h>

struct game {
        struct array    *renderers;
        u8              frame;
};

#endif
