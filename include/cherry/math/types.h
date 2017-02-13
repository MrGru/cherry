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
#ifndef __CHERRY_MATH_TYPES_H__
#define __CHERRY_MATH_TYPES_H__

union vec2 {
        struct { float x, y; };
        struct { float t, s; };
        float v[2];
} __attribute__((aligned(8))) ;

union vec3 {
        struct { float x, y, z; };
        struct { float r, g, b; };
        float v[3];
};

union vec4 {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        float v[4];
} __attribute__((aligned(16))) ;

union mat2 {
        struct { float m00, m01, m10, m11; };
        float m[4];
        float mm[2][2];
} __attribute__((aligned(16))) ;

union mat3 {
        struct { float m00, m01, m02, m10, m11, m12, m20, m21, m22; };
        float m[9];
        float mm[3][3];
};

union mat4 {
        struct { float m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33; };
        float m[16];
        float mm[4][4];
} __attribute__((aligned(16))) ;

#define devec4(p) (p).x, (p).y, (p).z, (p).w

#define devec3(p) (p).x, (p).y, (p).z

#define devec2(p) (p).x, (p).y

#endif
