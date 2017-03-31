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

#include <common/camera.slib>
#include <common/matrix.slib>

input vec2                      position;
input vec2                      texcoord;
input vec4                      color;

uniform camera_uniform          camera;
uniform transform_uniform       transform;

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;
output vec2     pixel_texcoord;

void main()
{
        gl_Position             = camera.project * camera.view * transform.transform * vec4(position, 0.0, 1.0);

        pixel_color             = color;
        pixel_texcoord          = texcoord;
}
