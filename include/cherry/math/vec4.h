/*
 * 4-dimensions vector operators implementaion
 */
#ifndef __CHERRY_MATH_VEC4_H__
#define __CHERRY_MATH_VEC4_H__

#include <cherry/math/types.h>
#include <math.h>

static inline union vec4 vec4(float *v)
{
        return (union vec4){v[0], v[1], v[2], v[3]};
}

static inline union vec4 vec4_add(union vec4 p1, union vec4 p2)
{
        return (union vec4){p1.x + p2.x, p1.y + p2.y, p1.z + p2.z, p1.w + p2.w};
}

static inline union vec4 vec4_sub(union vec4 p1, union vec4 p2)
{
        return (union vec4){p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, p1.w - p2.w};
}

static inline union vec4 vec4_mul(union vec4 p1, union vec4 p2)
{
        return (union vec4){p1.x * p2.x, p1.y * p2.y, p1.z * p2.z, p1.w * p2.w};
}

static inline union vec4 vec4_div(union vec4 p1, union vec4 p2)
{
        return (union vec4){p1.x / p2.x, p1.y / p2.y, p1.z / p2.z, p1.w / p2.w};
}

static inline union vec4 vec4_add_scalar(union vec4 p1, float n)
{
        return (union vec4){p1.x + n, p1.y + n, p1.z + n, p1.w + n};
}

static inline union vec4 vec4_sub_scalar(union vec4 p1, float n)
{
        return (union vec4){p1.x - n, p1.y - n, p1.z - n, p1.w - n};
}

static inline union vec4 vec4_mul_scalar(union vec4 p1, float n)
{
        return (union vec4){p1.x * n, p1.y * n, p1.z * n, p1.w * n};
}

static inline union vec4 vec4_div_scalar(union vec4 p1, float n)
{
        return (union vec4){p1.x / n, p1.y / n, p1.z / n, p1.w / n};
}

static inline float vec4_length(union vec4 p1)
{
        return (float)sqrt(p1.x * p1.x + p1.y * p1.y + p1.z * p1.z + p1.w * p1.w);
}

static inline float vec4_dot(union vec4 p1, union vec4 p2)
{
        return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z + p1.w * p2.w;
}

static inline float vec4_dst(union vec4 p1, union vec4 p2)
{
        return vec4_length(vec4_sub(p2, p1));
}

static inline union vec4 vec4_cross(union vec4 p1, union vec4 p2)
{
        return (union vec4){
                p1.y * p2.z - p1.z * p2.y,
                p1.z * p2.x - p1.x * p2.z,
                p1.x * p2.y - p2.y * p1.x,
                0.0f
        };
}

static inline union vec4 vec4_normalize(union vec4 p1)
{
        return vec4_mul_scalar(p1, 1.0f / vec4_length(p1));
}

#endif
