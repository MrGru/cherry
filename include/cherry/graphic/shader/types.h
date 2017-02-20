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
#include <cherry/math/types.h>

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
        SHADER_3D_COLOR_VIEW,
        SHADER_3D_COLOR_VIEW_POSITION,
        SHADER_3D_COLOR_DIR_LIGHT_0_DIRECTION,
        SHADER_3D_COLOR_DIR_LIGHT_0_AMBIENT,
        SHADER_3D_COLOR_DIR_LIGHT_0_DIFFUSE,
        SHADER_3D_COLOR_DIR_LIGHT_0_SPECULAR,
        SHADER_3D_COLOR_DIR_LIGHT_1_DIRECTION,
        SHADER_3D_COLOR_DIR_LIGHT_1_AMBIENT,
        SHADER_3D_COLOR_DIR_LIGHT_1_DIFFUSE,
        SHADER_3D_COLOR_DIR_LIGHT_1_SPECULAR,
        SHADER_3D_COLOR_DIR_LIGHT_2_DIRECTION,
        SHADER_3D_COLOR_DIR_LIGHT_2_AMBIENT,
        SHADER_3D_COLOR_DIR_LIGHT_2_DIFFUSE,
        SHADER_3D_COLOR_DIR_LIGHT_2_SPECULAR,
        SHADER_3D_COLOR_POINT_LIGHT_0_POSITION,
        SHADER_3D_COLOR_POINT_LIGHT_0_CONSTANT,
        SHADER_3D_COLOR_POINT_LIGHT_0_LINEAR,
        SHADER_3D_COLOR_POINT_LIGHT_0_QUADRATIC,
        SHADER_3D_COLOR_POINT_LIGHT_0_AMBIENT,
        SHADER_3D_COLOR_POINT_LIGHT_0_DIFFUSE,
        SHADER_3D_COLOR_POINT_LIGHT_0_SPECULAR,
        SHADER_3D_COLOR_POINT_LIGHT_1_POSITION,
        SHADER_3D_COLOR_POINT_LIGHT_1_CONSTANT,
        SHADER_3D_COLOR_POINT_LIGHT_1_LINEAR,
        SHADER_3D_COLOR_POINT_LIGHT_1_QUADRATIC,
        SHADER_3D_COLOR_POINT_LIGHT_1_AMBIENT,
        SHADER_3D_COLOR_POINT_LIGHT_1_DIFFUSE,
        SHADER_3D_COLOR_POINT_LIGHT_1_SPECULAR,
        SHADER_3D_COLOR_POINT_LIGHT_2_POSITION,
        SHADER_3D_COLOR_POINT_LIGHT_2_CONSTANT,
        SHADER_3D_COLOR_POINT_LIGHT_2_LINEAR,
        SHADER_3D_COLOR_POINT_LIGHT_2_QUADRATIC,
        SHADER_3D_COLOR_POINT_LIGHT_2_AMBIENT,
        SHADER_3D_COLOR_POINT_LIGHT_2_DIFFUSE,
        SHADER_3D_COLOR_POINT_LIGHT_2_SPECULAR,
        SHADER_3D_COLOR_SPOT_LIGHT_0_POSITION,
        SHADER_3D_COLOR_SPOT_LIGHT_0_DIRECTION,
        SHADER_3D_COLOR_SPOT_LIGHT_0_CUT_OFF,
        SHADER_3D_COLOR_SPOT_LIGHT_0_OUTER_CUT_OFF,
        SHADER_3D_COLOR_SPOT_LIGHT_0_CONSTANT,
        SHADER_3D_COLOR_SPOT_LIGHT_0_LINEAR,
        SHADER_3D_COLOR_SPOT_LIGHT_0_QUADRATIC,
        SHADER_3D_COLOR_SPOT_LIGHT_0_AMBIENT,
        SHADER_3D_COLOR_SPOT_LIGHT_0_DIFFUSE,
        SHADER_3D_COLOR_SPOT_LIGHT_0_SPECULAR
};

struct dlight3d {
        union vec3 direction;
        union vec3 ambient;
        union vec3 diffuse;
        union vec3 specular;
};

struct plight3d {
        union vec3      position;
        float           constant;
        float           linear;
        float           quadratic;
        union vec3      ambient;
        union vec3      diffuse;
        union vec3      specular;
};

struct slight3d {
        union vec3      position;
        union vec3      direction;
        float           cut_off;
        float           outer_cut_off;
        float           constant;
        float           linear;
        float           quadratic;
        union vec3      ambient;
        union vec3      diffuse;
        union vec3      specular;
};

struct shader_3d_color_uniform {
        union mat4              project;
        union mat4              view;
        union vec3              view_position;
        struct dlight3d         dlights[3];
        struct plight3d         plights[3];
        struct slight3d         slights[1];
} __attribute__((aligned (1024)));

#endif
