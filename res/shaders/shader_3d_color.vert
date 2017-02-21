/*
 * Copyright (C) 2017 Manh Tran
 *
 * quick approach for allocation and free memory
 * single thread - need implementation for concurrent
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
#version 100

#ifdef GL_ES
        #define input   attribute
        #define output  varying
#else
        #define input   in
        #define output  out
#endif

/*
 * z used to sort instances by depth test function after fragment step
 * vid used to access current texcoord, vertex
 */
input float     vid;
input vec4      color;
input mat4      transform;
input vec3      vertex_1;
input vec3      vertex_2;
input vec3      vertex_3;
input vec3      normal_1;
input vec3      normal_2;
input vec3      normal_3;
input vec3      vertex_color;

/*
 * combine vertex_i, normal_i, vertex_color into array to access from vid
 */
vec3            vertice[3];
vec3            normals[3];
highp int       vertex_colors[3];

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;
output vec3     pixel_normal;
output vec3     pixel_frag_pos;

uniform mat4    project;
uniform mat4    view;

/*
 * see @pack_rgb_to_float in gfx/graphic.c for more information
 */
vec4 decodeFloatColor(highp int val)
{
        vec4 c;
        c.r = float(val / 65536 + 128);
        c.g = float((val * 65536) / 16777216 + 128);
        c.b = float((val * 16777216) / 16777216 + 128);
        c.a = 255.0;
        c /= 255.0;
        return c;
}

void main()
{
        /*
         * group vertice
         */
        vertice[0]              = vertex_1;
        vertice[1]              = vertex_2;
        vertice[2]              = vertex_3;

        /*
         * group normal
         */
        normals[0]              = normal_1;
        normals[1]              = normal_2;
        normals[2]              = normal_3;
        /*
         * group vertex_colors
         */
        vertex_colors[0]        = int(vertex_color.r);
        vertex_colors[1]        = int(vertex_color.g);
        vertex_colors[2]        = int(vertex_color.b);

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec3 pos                = vertice[int(vid)];
        gl_Position             = project * view * transform * vec4(pos, 1.0);

        /*
         * pass pixel parameters
         */
        pixel_normal            = normals[int(vid)];
        /* use this normal calculation below in case vertex is scaled
         * matrix4_transpose and matrix4_inverse found in temp_functions
         * pixel_normal    = mat3(matrix4_transpose(matrix4_inverse(transform))) * normals[int(vid)];
         */
        pixel_frag_pos          = vec3(transform * vec4(pos, 1.0));
        pixel_color             = color * decodeFloatColor(vertex_colors[int(vid)]);
}
