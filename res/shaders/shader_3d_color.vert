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

/*
 * combine vertex_i into array to access from vid
 */
vec3            vertice[3];

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;

uniform mat4    project;
uniform mat4    view;

void main()
{
        /*
         * group vertice
         */
        vertice[0]      = vertex_1;
        vertice[1]      = vertex_2;
        vertice[2]      = vertex_3;

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec3 pos        = vertice[int(vid)];
        gl_Position     = project * view * transform * vec4(pos, 1.0);
        /*
         * pass pixel parameters
         */
        pixel_color     = color;
}
