#ifndef __CHERRY_GRAPHIC_SHADER_TYPES_H__
#define __CHERRY_GRAPHIC_SHADER_TYPES_H__

#include <cherry/graphic/types.h>

/*
 * shader color uniforms
 */
enum {
        SHADER_COLOR_PROJECT,
        SHADER_COLOR_VIEW
};

struct shader_color_uniform {
        union mat4 project;
        union mat4 view;
};

/*
 * shader texture uniforms
 */
enum {
        SHADER_TEXTURE_PROJECT,
        SHADER_TEXTURE_VIEW,

        SHADER_TEXTURE_IMAGE_0,
        SHADER_TEXTURE_IMAGE_N = SHADER_TEXTURE_IMAGE_0 + 8
};

struct shader_texture_uniform {
        union mat4 project;
        union mat4 view;
};

#endif
