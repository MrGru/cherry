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
#ifndef __CHERRY_MATH_VEC2_H__
#define __CHERRY_MATH_VEC2_H__

#include <cherry/math/types.h>
#include <math.h>

static inline union vec2 vec2(float *v)
{
        return (union vec2){v[0], v[1]};
}

static inline union vec2 vec2_add(union vec2 p1, union vec2 p2)
{
        return (union vec2){p1.x + p2.x, p1.y + p2.y};
}

static inline union vec2 vec2_sub(union vec2 p1, union vec2 p2)
{
        return (union vec2){p1.x - p2.x, p1.y - p2.y};
}

static inline union vec2 vec2_mul(union vec2 p1, union vec2 p2)
{
        return (union vec2){p1.x * p2.x, p1.y * p2.y};
}

static inline union vec2 vec2_div(union vec2 p1, union vec2 p2)
{
        return (union vec2){p1.x / p2.x, p1.y / p2.y};
}

static inline union vec2 vec2_add_scalar(union vec2 p1, float n)
{
        return (union vec2){p1.x + n, p1.y + n};
}

static inline union vec2 vec2_sub_scalar(union vec2 p1, float n)
{
        return (union vec2){p1.x - n, p1.y - n};
}

static inline union vec2 vec2_mul_scalar(union vec2 p1, float n)
{
        return (union vec2){p1.x * n, p1.y * n};
}

static inline union vec2 vec2_div_scalar(union vec2 p1, float n)
{
        return (union vec2){p1.x / n, p1.y / n};
}

static inline float vec2_length(union vec2 p1)
{
        return (float)sqrt(p1.x * p1.x + p1.y * p1.y);
}

static inline float vec2_dot(union vec2 p1, union vec2 p2)
{
        return p1.x * p2.x + p1.y * p2.y;
}

static inline float vec2_dst(union vec2 p1, union vec2 p2)
{
        return vec2_length(vec2_sub(p2, p1));
}

static inline union vec2 vec2_normalize(union vec2 p1)
{
        return vec2_mul_scalar(p1, 1.0f / vec2_length(p1));
}

#endif
