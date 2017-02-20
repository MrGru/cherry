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

/*
 * combine vertex_i, normal_i into array to access from vid
 */
vec3            vertice[3];
vec3            normals[3];

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;
output vec3     pixel_normal;
output vec3     pixel_frag_pos;

uniform mat4    project;
uniform mat4    view;

mat4 matrix4_inverse(mat4 matrix)
{
        mat4 inv;
        float det;

        inv[0].x = matrix[1].y  * matrix[2].z * matrix[3].w -
                matrix[1].y  * matrix[2].w * matrix[3].z -
                matrix[2].y  * matrix[1].z  * matrix[3].w +
                matrix[2].y  * matrix[1].w  * matrix[3].z +
                matrix[3].y * matrix[1].z  * matrix[2].w -
                matrix[3].y * matrix[1].w  * matrix[2].z;

        inv[1].x = -matrix[1].x  * matrix[2].z * matrix[3].w +
                matrix[1].x  * matrix[2].w * matrix[3].z +
                matrix[2].x  * matrix[1].z  * matrix[3].w -
                matrix[2].x  * matrix[1].w  * matrix[3].z -
                matrix[3].x * matrix[1].z  * matrix[2].w +
                matrix[3].x * matrix[1].w  * matrix[2].z;

        inv[2].x = matrix[1].x  * matrix[2].y * matrix[3].w -
                matrix[1].x  * matrix[2].w * matrix[3].y -
                matrix[2].x  * matrix[1].y * matrix[3].w +
                matrix[2].x  * matrix[1].w * matrix[3].y +
                matrix[3].x * matrix[1].y * matrix[2].w -
                matrix[3].x * matrix[1].w * matrix[2].y;

        inv[3].x =  -matrix[1].x  * matrix[2].y * matrix[3].z +
                matrix[1].x  * matrix[2].z * matrix[3].y +
                matrix[2].x  * matrix[1].y * matrix[3].z -
                matrix[2].x  * matrix[1].z * matrix[3].y -
                matrix[3].x * matrix[1].y * matrix[2].z +
                matrix[3].x * matrix[1].z * matrix[2].y;

        inv[0].y = -matrix[0].y  * matrix[2].z * matrix[3].w +
              matrix[0].y  * matrix[2].w * matrix[3].z +
              matrix[2].y  * matrix[0].z * matrix[3].w -
              matrix[2].y  * matrix[0].w * matrix[3].z -
              matrix[3].y * matrix[0].z * matrix[2].w +
              matrix[3].y * matrix[0].w * matrix[2].z;

        inv[1].y =  matrix[0].x  * matrix[2].z * matrix[3].w -
              matrix[0].x  * matrix[2].w * matrix[3].z -
              matrix[2].x  * matrix[0].z * matrix[3].w +
              matrix[2].x  * matrix[0].w * matrix[3].z +
              matrix[3].x * matrix[0].z * matrix[2].w -
              matrix[3].x * matrix[0].w * matrix[2].z;

        inv[2].y = -matrix[0].x  * matrix[2].y * matrix[3].w +
              matrix[0].x  * matrix[2].w * matrix[3].y +
              matrix[2].x  * matrix[0].y * matrix[3].w -
              matrix[2].x  * matrix[0].w * matrix[3].y -
              matrix[3].x * matrix[0].y * matrix[2].w +
              matrix[3].x * matrix[0].w * matrix[2].y;

        inv[3].y = matrix[0].x  * matrix[2].y * matrix[3].z -
              matrix[0].x  * matrix[2].z * matrix[3].y -
              matrix[2].x  * matrix[0].y * matrix[3].z +
              matrix[2].x  * matrix[0].z * matrix[3].y +
              matrix[3].x * matrix[0].y * matrix[2].z -
              matrix[3].x * matrix[0].z * matrix[2].y;

        inv[0].z =  matrix[0].y  * matrix[1].z * matrix[3].w -
              matrix[0].y  * matrix[1].w * matrix[3].z -
              matrix[1].y  * matrix[0].z * matrix[3].w +
              matrix[1].y  * matrix[0].w * matrix[3].z +
              matrix[3].y * matrix[0].z * matrix[1].w -
              matrix[3].y * matrix[0].w * matrix[1].z;

        inv[1].z = -matrix[0].x  * matrix[1].z * matrix[3].w +
              matrix[0].x  * matrix[1].w * matrix[3].z +
              matrix[1].x  * matrix[0].z * matrix[3].w -
              matrix[1].x  * matrix[0].w * matrix[3].z -
              matrix[3].x * matrix[0].z * matrix[1].w +
              matrix[3].x * matrix[0].w * matrix[1].z;

        inv[2].z = matrix[0].x  * matrix[1].y * matrix[3].w -
              matrix[0].x  * matrix[1].w * matrix[3].y -
              matrix[1].x  * matrix[0].y * matrix[3].w +
              matrix[1].x  * matrix[0].w * matrix[3].y +
              matrix[3].x * matrix[0].y * matrix[1].w -
              matrix[3].x * matrix[0].w * matrix[1].y;

        inv[3].z =  -matrix[0].x  * matrix[1].y * matrix[3].z +
                matrix[0].x  * matrix[1].z * matrix[3].y +
                matrix[1].x  * matrix[0].y * matrix[3].z -
                matrix[1].x  * matrix[0].z * matrix[3].y -
                matrix[3].x * matrix[0].y * matrix[1].z +
                matrix[3].x * matrix[0].z * matrix[1].y;

        inv[0].w = -matrix[0].y * matrix[1].z * matrix[2].w +
              matrix[0].y * matrix[1].w * matrix[2].z +
              matrix[1].y * matrix[0].z * matrix[2].w -
              matrix[1].y * matrix[0].w * matrix[2].z -
              matrix[2].y * matrix[0].z * matrix[1].w +
              matrix[2].y * matrix[0].w * matrix[1].z;

        inv[1].w =  matrix[0].x * matrix[1].z * matrix[2].w -
              matrix[0].x * matrix[1].w * matrix[2].z -
              matrix[1].x * matrix[0].z * matrix[2].w +
              matrix[1].x * matrix[0].w * matrix[2].z +
              matrix[2].x * matrix[0].z * matrix[1].w -
              matrix[2].x * matrix[0].w * matrix[1].z;

        inv[2].w =  -matrix[0].x * matrix[1].y * matrix[2].w +
                matrix[0].x * matrix[1].w * matrix[2].y +
                matrix[1].x * matrix[0].y * matrix[2].w -
                matrix[1].x * matrix[0].w * matrix[2].y -
                matrix[2].x * matrix[0].y * matrix[1].w +
                matrix[2].x * matrix[0].w * matrix[1].y;

        inv[3].w = matrix[0].x * matrix[1].y * matrix[2].z -
              matrix[0].x * matrix[1].z * matrix[2].y -
              matrix[1].x * matrix[0].y * matrix[2].z +
              matrix[1].x * matrix[0].z * matrix[2].y +
              matrix[2].x * matrix[0].y * matrix[1].z -
              matrix[2].x * matrix[0].z * matrix[1].y;

        det = matrix[0].x * inv[0].x + matrix[0].y * inv[1].x + matrix[0].z * inv[2].x + matrix[0].w * inv[3].x;

        det = clamp(det, 0.0000001, 999999.0);

        det = 1.0 / det;

        inv = inv * det;

        return inv;
}


mat4 matrix4_transpose(mat4 matrix)
{
        mat4 m = mat4(matrix[0].x, matrix[1].x, matrix[2].x, matrix[3].x,
                matrix[0].y, matrix[1].y, matrix[2].y, matrix[3].y,
                matrix[0].z, matrix[1].z, matrix[2].z, matrix[3].z,
                matrix[0].w, matrix[1].w, matrix[2].w, matrix[3].w);
        return m;
}

void main()
{
        /*
         * group vertice
         */
        vertice[0]      = vertex_1;
        vertice[1]      = vertex_2;
        vertice[2]      = vertex_3;

        /*
         * group normal
         */
        normals[0]      = normal_1;
        normals[1]      = normal_2;
        normals[2]      = normal_3;

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec3 pos        = vertice[int(vid)];
        gl_Position     = project * view * transform * vec4(pos, 1.0);

        /*
         * pass pixel parameters
         */
        pixel_normal    = normals[int(vid)];
        /* use this normal calculation below in case vertex is scaled
         * pixel_normal    = mat3(matrix4_transpose(matrix4_inverse(transform))) * normals[int(vid)]; 
         */
        pixel_frag_pos  = vec3(transform * vec4(pos, 1.0));
        pixel_color     = color;
}
