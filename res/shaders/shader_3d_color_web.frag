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

input vec4      pixel_color;
input float     pixel_texid;
input vec2      pixel_texcoord;

/*
 * almost mobile devices allow only 8 concurrent sampler2Ds as maximum
 */
uniform sampler2D       image[8];

void main()
{
        vec4 pixel;

        if(pixel_texid >= 7.0) {
                pixel = get_pixel(image[7], pixel_texcoord);
        }

        if(pixel_texid >= 6.0) {
                pixel = get_pixel(image[6], pixel_texcoord);
        }

        if(pixel_texid >= 5.0) {
                pixel = get_pixel(image[5], pixel_texcoord);
        }

        if(pixel_texid >= 4.0) {
                pixel = get_pixel(image[4], pixel_texcoord);
        }

        if(pixel_texid >= 3.0) {
                pixel = get_pixel(image[3], pixel_texcoord);
        }

        if(pixel_texid >= 2.0) {
                pixel = get_pixel(image[2], pixel_texcoord);
        }

        if(pixel_texid >= 1.0) {
                pixel = get_pixel(image[1], pixel_texcoord);
        }

        if(pixel_texid >= 0.0) {
                pixel = get_pixel(image[0], pixel_texcoord);
        }

        out_pixel = pixel_color * pixel;
}
