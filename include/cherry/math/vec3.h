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
#ifndef __CHERRY_MATH_VEC3_H__
#define __CHERRY_MATH_VEC3_H__

#include <cherry/math/types.h>
#include <math.h>

static inline union vec3 vec3(float *v)
{
        return (union vec3){v[0], v[1], v[2]};
}

static inline union vec3 vec3_neg(union vec3 p1)
{
        return (union vec3){-p1.x, -p1.y, -p1.z};
}

static inline union vec3 vec3_add(union vec3 p1, union vec3 p2)
{
        return (union vec3){p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
}

static inline union vec3 vec3_sub(union vec3 p1, union vec3 p2)
{
        return (union vec3){p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
}

static inline union vec3 vec3_mul(union vec3 p1, union vec3 p2)
{
        return (union vec3){p1.x * p2.x, p1.y * p2.y, p1.z * p2.z};
}

static inline union vec3 vec3_div(union vec3 p1, union vec3 p2)
{
        return (union vec3){p1.x / p2.x, p1.y / p2.y, p1.z / p2.z};
}

static inline union vec3 vec3_add_scalar(union vec3 p1, float n)
{
        return (union vec3){p1.x + n, p1.y + n, p1.z + n};
}

static inline union vec3 vec3_sub_scalar(union vec3 p1, float n)
{
        return (union vec3){p1.x - n, p1.y - n, p1.z - n};
}

static inline union vec3 vec3_mul_scalar(union vec3 p1, float n)
{
        return (union vec3){p1.x * n, p1.y * n, p1.z * n};
}

static inline union vec3 vec3_div_scalar(union vec3 p1, float n)
{
        return (union vec3){p1.x / n, p1.y / n, p1.z / n};
}

static inline float vec3_length(union vec3 p1)
{
        return (float)sqrt(p1.x * p1.x + p1.y * p1.y + p1.z * p1.z);
}

static inline float vec3_dot(union vec3 p1, union vec3 p2)
{
        return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

static inline float vec3_dst(union vec3 p1, union vec3 p2)
{
        return vec3_length(vec3_sub(p2, p1));
}

static inline union vec3 vec3_cross(union vec3 p1, union vec3 p2)
{
        return (union vec3){
                p1.y * p2.z - p1.z * p2.y,
                p1.z * p2.x - p1.x * p2.z,
                p1.x * p2.y - p2.y * p1.x
        };
}

static inline union vec3 vec3_normalize(union vec3 p1)
{
        return vec3_mul_scalar(p1, 1.0f / vec3_length(p1));
}

#endif
