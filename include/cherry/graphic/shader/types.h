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

/*
 * shader 3d color uniforms
 */
 enum {
         SHADER_3D_COLOR_PROJECT,
         SHADER_3D_COLOR_VIEW
 };

 struct shader_3d_color_uniform {
         union mat4 project;
         union mat4 view;
 };

#endif
