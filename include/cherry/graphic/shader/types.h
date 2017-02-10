#ifndef __CHERRY_GRAPHIC_SHADER_TYPES_H__
#define __CHERRY_GRAPHIC_SHADER_TYPES_H__

#include <cherry/graphic/types.h>

enum {
        SHADER_COLOR_PROJECT,
        SHADER_COLOR_VIEW
};

struct shader_color_uniform {
        union mat4 project;
        union mat4 view;
};

#endif
