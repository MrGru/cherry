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
        precision highp float;
        #define input           varying
        #define get_pixel       texture2D
        #define out_pixel       gl_FragColor
#else
        #define input           in
        #define get_pixel       texture
        out vec4                fragColor;
        #define out_pixel       fragColor
#endif

input vec4              pixel_color;
input vec2              pixel_coord;
input float             pixel_texid;

/*
 * almost mobile devices allow only 8 concurrent sampler2Ds as maximum
 */
uniform sampler2D       image[8];

void main()
{
        out_pixel = pixel_color *  get_pixel(image[int(pixel_texid)], pixel_coord);
}
