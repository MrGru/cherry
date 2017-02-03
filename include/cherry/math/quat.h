/*
 * quaternion implementaion
 */
#ifndef __CHERRY_MATH_QUAT_H__
#define __CHERRY_MATH_QUAT_H__

#include <cherry/math/mat4.h>
#include <cherry/math/vec4.h>
#include <math.h>

extern union vec4 quat_identity;

static inline union vec4 quat_angle_axis(float radian, float *v)
{
        float half = radian * 0.5f;
        float scale = sinf(half);
        return (union vec4){scale * v[0], scale * v[1], scale * v[2], cosf(half)};
}

static inline union vec4 quat_mul(union vec4 q1, union vec4 q2)
{
        return (union vec4) {
                q1.v[3] * q2.v[0] +
                q1.v[0] * q2.v[3] +
                q1.v[1] * q2.v[2] -
                q1.v[2] * q2.v[1],

                q1.v[3] * q2.v[1] +
                q1.v[1] * q2.v[3] +
                q1.v[2] * q2.v[0] -
                q1.v[0] * q2.v[2],

                q1.v[3] * q2.v[2] +
                q1.v[2] * q2.v[3] +
                q1.v[0] * q2.v[1] -
                q1.v[1] * q2.v[0],

                q1.v[3] * q2.v[3] -
                q1.v[0] * q2.v[0] -
                q1.v[1] * q2.v[1] -
                q1.v[2] * q2.v[2]
        };
}

static inline union vec4 quat_from_mat4(union mat4 m)
{
        float tr =  m.m[0] + m.m[5] + m.m[10];
        union vec4 ret;
        if(tr > 0)
        {
                float s = sqrt(tr + 1.0) * 2;
                ret.v[3] = 0.25 * s;
                ret.v[0] = (m.m[9] - m.m[6]) / s;
                ret.v[1] = (m.m[2] - m.m[8]) / s;
                ret.v[2] = (m.m[4] - m.m[1]) / s;
        }
        else if(m.m[0] > m.m[5] && m.m[0] > m.m[10])
        {
                float s = sqrt(1.0 + m.m[0] - m.m[5] - m.m[10]) * 2;
                ret.v[0] = 0.25 * s;
                ret.v[1] = (m.m[4] + m.m[1]) / s;
                ret.v[2] = (m.m[2] + m.m[8]) / s;
                ret.v[3] = (m.m[9] - m.m[6]) / s;
        }
        else if(m.m[5] > m.m[10])
        {
                float s = sqrt(1.0 + m.m[5] - m.m[0] - m.m[10]) * 2;
                ret.v[0] = (m.m[4] + m.m[1]) / s;
                ret.v[1] = 0.25 * s;
                ret.v[2] = (m.m[9] + m.m[6]) / s;
                ret.v[3] = (m.m[2] - m.m[8]) / s;
        }
        else
        {
                float s = sqrt(1.0 + m.m[10] - m.m[0] - m.m[5]) * 2;
                ret.v[0] = (m.m[2] + m.m[8]) / s;
                ret.v[1] = (m.m[9] + m.m[6]) / s;
                ret.v[2] = 0.25 * s;
                ret.v[3] = (m.m[4] - m.m[1]) / s;
        }
        return ret;
}

#endif
