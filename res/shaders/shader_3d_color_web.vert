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

#include <common/light.slib>
#include <common/matrix.slib>
#include <common/decode.slib>

/*
 * z used to sort instances by depth test function after fragment step
 * vid used to access current texcoord, vertex
 */
input float     vid;
input float     bright;
input mat4      transform;
input vec3      vertex_1;
input vec3      vertex_2;
input vec3      vertex_3;
input vec3      normal_1;
input vec3      normal_2;
input vec3      normal_3;
input vec4      vertex_color;
input vec4      texcoord;

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;
output float    pixel_texid;
output vec2     pixel_texcoord;

uniform mat4    project;
uniform mat4    view;

/*
 * we need camera position
 */
uniform vec3    view_position;

/*
 * WebGL currently does not support array index as attribute so I need use branch
 */
void main()
{
        vec4 alpha              = decodeFloatColor(int(vertex_color[3]));
        vec4 decodecolor;
        if(vid >= 2.0) {
                decodecolor        = decodeFloatColor(int(vertex_color[2]));
                decodecolor.a           = alpha[2];
        } else if(vid >= 1.0) {
                decodecolor        = decodeFloatColor(int(vertex_color[1]));
                decodecolor.a           = alpha[1];
        } else {
                decodecolor        = decodeFloatColor(int(vertex_color[0]));
                decodecolor.a           = alpha[0];
        }

        vec2 tc;
        if(vid >= 2.0) {
                tc      = decodeTexcoord(texcoord[2]);
        } else if(vid >= 1.0) {
                tc      = decodeTexcoord(texcoord[1]);
        } else {
                tc      = decodeTexcoord(texcoord[0]);
        }

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec3 pos;
        if(vid >= 2.0) {
                pos                = vertex_3;
        } else if(vid >= 1.0) {
                pos                = vertex_2;
        } else {
                pos                = vertex_1;
        }
        gl_Position             = project * view * transform * vec4(pos, 1.0);

        /*
         * pass pixel parameters
         */
        vec3 pixel_normal;
        if(vid >= 2.0) {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_3;
        } else if(vid >= 1.0) {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_2;
        } else {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_1;
        }

        vec3 norm               = normalize(pixel_normal);

        vec3 result             = calculate_light(transform, view_position, pos, norm, bright);

        pixel_color             = decodecolor * vec4(result, 1.0);
        pixel_texcoord          = tc;
        pixel_texid             = texcoord[3];
}
