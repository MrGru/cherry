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
#ifndef __CHERRY_MATH_MAT4_H__
#define __CHERRY_MATH_MAT4_H__

#include <cherry/math/types.h>
#include <math.h>

extern union mat4 mat4_identity;

static inline union mat4 mat4(float *m)
{
        return (union mat4){
                m[0], m[1], m[2], m[3],
                m[4], m[5], m[6], m[7],
                m[8], m[9], m[10],m[11],
                m[12],m[13],m[14],m[15]
        };
}

static inline union mat4 mat4_transpose(float *m)
{
        return (union mat4){
                m[0], m[4], m[8], m[12],
                m[1], m[5], m[9], m[13],
                m[2], m[6], m[10],m[14],
                m[3], m[7], m[11],m[15]
        };
}

static inline union mat4 mat4_mul(union mat4 m1, union mat4 m2)
{
        union mat4 m;

        m.m[0]  = m1.m[0] * m2.m[0]  + m1.m[4] * m2.m[1]  + m1.m[8] * m2.m[2]   + m1.m[12] * m2.m[3];
        m.m[4]  = m1.m[0] * m2.m[4]  + m1.m[4] * m2.m[5]  + m1.m[8] * m2.m[6]   + m1.m[12] * m2.m[7];
        m.m[8]  = m1.m[0] * m2.m[8]  + m1.m[4] * m2.m[9]  + m1.m[8] * m2.m[10]  + m1.m[12] * m2.m[11];
        m.m[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8] * m2.m[14]  + m1.m[12] * m2.m[15];

        m.m[1]  = m1.m[1] * m2.m[0]  + m1.m[5] * m2.m[1]  + m1.m[9] * m2.m[2]   + m1.m[13] * m2.m[3];
        m.m[5]  = m1.m[1] * m2.m[4]  + m1.m[5] * m2.m[5]  + m1.m[9] * m2.m[6]   + m1.m[13] * m2.m[7];
        m.m[9]  = m1.m[1] * m2.m[8]  + m1.m[5] * m2.m[9]  + m1.m[9] * m2.m[10]  + m1.m[13] * m2.m[11];
        m.m[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9] * m2.m[14]  + m1.m[13] * m2.m[15];

        m.m[2]  = m1.m[2] * m2.m[0]  + m1.m[6] * m2.m[1]  + m1.m[10] * m2.m[2]  + m1.m[14] * m2.m[3];
        m.m[6]  = m1.m[2] * m2.m[4]  + m1.m[6] * m2.m[5]  + m1.m[10] * m2.m[6]  + m1.m[14] * m2.m[7];
        m.m[10] = m1.m[2] * m2.m[8]  + m1.m[6] * m2.m[9]  + m1.m[10] * m2.m[10] + m1.m[14] * m2.m[11];
        m.m[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];

        m.m[3]  = m1.m[3] * m2.m[0]  + m1.m[7] * m2.m[1]  + m1.m[11] * m2.m[2]  + m1.m[15] * m2.m[3];
        m.m[7]  = m1.m[3] * m2.m[4]  + m1.m[7] * m2.m[5]  + m1.m[11] * m2.m[6]  + m1.m[15] * m2.m[7];
        m.m[11] = m1.m[3] * m2.m[8]  + m1.m[7] * m2.m[9]  + m1.m[11] * m2.m[10] + m1.m[15] * m2.m[11];
        m.m[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];

        return m;
}


static inline union mat4 mat4_new_translation(union vec3 v)
{
        union mat4 m = mat4_identity;
        m.m[12] = v.x;
        m.m[13] = v.y;
        m.m[14] = v.z;
        return m;
}

static inline union mat4 mat4_new_scale(union vec3 v)
{
        union mat4 m = mat4_identity;
        m.m[0]  = v.x;
        m.m[5]  = v.y;
        m.m[10] = v.z;
        return m;
}

static inline union mat4 mat4_new_rotation(float radians, union vec3 v)
{
        v = vec3_normalize(v);
        float cos = cosf(radians);
        float cosp = 1.0f - cos;
        float sin = sinf(radians);

        return (union mat4) {
                cos + cosp * v.v[0] * v.v[0],
                cosp * v.v[0] * v.v[1] + v.v[2] * sin,
                cosp * v.v[0] * v.v[2] - v.v[1] * sin,
                0.0f,
                cosp * v.v[0] * v.v[1] - v.v[2] * sin,
                cos + cosp * v.v[1] * v.v[1],
                cosp * v.v[1] * v.v[2] + v.v[0] * sin,
                0.0f,
                cosp * v.v[0] * v.v[2] + v.v[1] * sin,
                cosp * v.v[1] * v.v[2] - v.v[0] * sin,
                cos + cosp * v.v[2] * v.v[2],
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f
        };
}

static inline union mat4 mat4_new_x_rotation(float radians)
{
        float cos = cosf(radians);
        float sin = sinf(radians);

        return (union mat4) {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cos, sin, 0.0f,
                0.0f, -sin, cos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
}

static inline union mat4 mat4_new_y_rotation(float radians)
{
        float cos = cosf(radians);
        float sin = sinf(radians);

        return (union mat4) {
                cos, 0.0f, -sin, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                sin, 0.0f, cos, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
}

static inline union mat4 mat4_new_z_rotation(float radians)
{
        float cos = cosf(radians);
        float sin = sinf(radians);

        return (union mat4) {
                cos, sin, 0.0f, 0.0f,
                -sin, cos, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
}

static inline union mat4 mat4_new_perspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
        float cotan = 1.0f / tanf(fovyRadians / 2.0f);
        return (union mat4) {
                cotan / aspect, 0.0f, 0.0f, 0.0f,
                0.0f, cotan, 0.0f, 0.0f,
                0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
                0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f
        };
}

static inline union mat4 mat4_new_ortho( float left, float right,
                                         float bottom, float top,
                                         float nearZ, float farZ)
{
        float ral = right + left;
        float rsl = right - left;
        float tab = top + bottom;
        float tsb = top - bottom;
        float fan = farZ + nearZ;
        float fsn = farZ - nearZ;

        return (union mat4){
                2.0f / rsl, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / tsb, 0.0f, 0.0f,
                0.0f, 0.0f, -2.0f / fsn, 0.0f,
                -ral / rsl, -tab / tsb, -fan / fsn, 1.0f
        };
}

static inline union mat4 mat4_new_look_at( float eyeX, float eyeY, float eyeZ,
                                           float centerX, float centerY, float centerZ,
                                           float upX, float upY, float upZ)
{
    union vec3 ev = { eyeX, eyeY, eyeZ };
    union vec3 cv = { centerX, centerY, centerZ };
    union vec3 uv = { upX, upY, upZ };
    union vec3 n = vec3_normalize(vec3_add(ev, vec3_neg(cv)));
    union vec3 u = vec3_normalize(vec3_cross(uv, n));
    union vec3 v = vec3_cross(n, u);

        return (union mat4){
                u.v[0], v.v[0], n.v[0], 0.0f,
                u.v[1], v.v[1], n.v[1], 0.0f,
                u.v[2], v.v[2], n.v[2], 0.0f,
                vec3_dot(vec3_neg(u), ev),
                vec3_dot(vec3_neg(v), ev),
                vec3_dot(vec3_neg(n), ev),
                1.0f
        };
}

static inline union mat4 mat4_translate(union mat4 m1, union vec3 p1)
{
        return (union mat4){
                m1.m[0], m1.m[1], m1.m[2], m1.m[3],
                m1.m[4], m1.m[5], m1.m[6], m1.m[7],
                m1.m[8], m1.m[9], m1.m[10], m1.m[11],
                m1.m[0] * p1.v[0] + m1.m[4] * p1.v[1] + m1.m[8] * p1.v[2] + m1.m[12],
                m1.m[1] * p1.v[0] + m1.m[5] * p1.v[1] + m1.m[9] * p1.v[2] + m1.m[13],
                m1.m[2] * p1.v[0] + m1.m[6] * p1.v[1] + m1.m[10] * p1.v[2] + m1.m[14],
                m1.m[15]
         };
}

static inline union mat4 mat4_scale(union mat4 m1, union vec3 p1)
{
        return (union mat4) {
                m1.m[0] * p1.v[0], m1.m[1] * p1.v[0], m1.m[2] * p1.v[0], m1.m[3] * p1.v[0],
                m1.m[4] * p1.v[1], m1.m[5] * p1.v[1], m1.m[6] * p1.v[1], m1.m[7] * p1.v[1],
                m1.m[8] * p1.v[2], m1.m[9] * p1.v[2], m1.m[10] * p1.v[2], m1.m[11] * p1.v[2],
                m1.m[12], m1.m[13], m1.m[14], m1.m[15]
        };
}

static inline union mat4 mat4_rotate(union mat4 m1, float radian, union vec3 p1)
{
        return mat4_mul(m1, mat4_new_rotation(radian, p1));
}

static inline union mat4 mat4_rotate_x(union mat4 m1, float radian)
{
        return mat4_mul(m1, mat4_new_x_rotation(radian));
}

static inline union mat4 mat4_rotate_y(union mat4 m1, float radian)
{
        return mat4_mul(m1, mat4_new_y_rotation(radian));
}

static inline union mat4 mat4_rotate_z(union mat4 m1, float radian)
{
        return mat4_mul(m1, mat4_new_z_rotation(radian));
}

static inline union vec4 mat4_mul_vec4(union mat4 m1, union vec4 p1)
{
        return (union vec4){
                m1.m[0] * p1.v[0] + m1.m[4] * p1.v[1] + m1.m[8] * p1.v[2] + m1.m[12] * p1.v[3],
                m1.m[1] * p1.v[0] + m1.m[5] * p1.v[1] + m1.m[9] * p1.v[2] + m1.m[13] * p1.v[3],
                m1.m[2] * p1.v[0] + m1.m[6] * p1.v[1] + m1.m[10] * p1.v[2] + m1.m[14] * p1.v[3],
                m1.m[3] * p1.v[0] + m1.m[7] * p1.v[1] + m1.m[11] * p1.v[2] + m1.m[15] * p1.v[3]
        };
}

static inline union vec3 mat4_mul_vec3_translation(union mat4 m1, union vec3 p1)
{
        union vec4 p = mat4_mul_vec4(m1, (union vec4){p1.x, p1.y, p1.z, 1.0f});
        return (union vec3){p.x, p.y, p.z};
}

static inline union vec3 mat4_mul_project_vec3(union mat4 m1, union vec3 p1)
{
        union vec4 p = mat4_mul_vec4(m1, (union vec4){p1.x, p1.y, p1.z, 1.0f});
        return vec3_mul_scalar((union vec3){p.x, p.y, p.z}, 1.0f / p.w);
}

static inline union mat4 mat4_from_quat(union vec4 q)
{
        q = vec4_normalize(q);

        float x = q.x;
        float y = q.y;
        float z = q.z;
        float w = q.w;

        float _2x = x + x;
        float _2y = y + y;
        float _2z = z + z;
        float _2w = w + w;

        return (union mat4) {
                1.0f - _2y * y - _2z * z,
                _2x * y + _2w * z,
                _2x * z - _2w * y,
                0.0f,
                _2x * y - _2w * z,
                1.0f - _2x * x - _2z * z,
                _2y * z + _2w * x,
                0.0f,
                _2x * z + _2w * y,
                _2y * z - _2w * x,
                1.0f - _2x * x - _2y * y,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f
        };
}

static inline union mat4 mat4_invert(union mat4 matrix)
{
  union mat4 inv = mat4_identity;
  float det;
  int i;

  inv.m[0] =  matrix.m[5]  * matrix.m[10] * matrix.m[15] -
              matrix.m[5]  * matrix.m[11] * matrix.m[14] -
              matrix.m[9]  * matrix.m[6]  * matrix.m[15] +
              matrix.m[9]  * matrix.m[7]  * matrix.m[14] +
              matrix.m[13] * matrix.m[6]  * matrix.m[11] -
              matrix.m[13] * matrix.m[7]  * matrix.m[10];

  inv.m[4] = -matrix.m[4]  * matrix.m[10] * matrix.m[15] +
              matrix.m[4]  * matrix.m[11] * matrix.m[14] +
              matrix.m[8]  * matrix.m[6]  * matrix.m[15] -
              matrix.m[8]  * matrix.m[7]  * matrix.m[14] -
              matrix.m[12] * matrix.m[6]  * matrix.m[11] +
              matrix.m[12] * matrix.m[7]  * matrix.m[10];

  inv.m[8] =  matrix.m[4]  * matrix.m[9] * matrix.m[15] -
              matrix.m[4]  * matrix.m[11] * matrix.m[13] -
              matrix.m[8]  * matrix.m[5] * matrix.m[15] +
              matrix.m[8]  * matrix.m[7] * matrix.m[13] +
              matrix.m[12] * matrix.m[5] * matrix.m[11] -
              matrix.m[12] * matrix.m[7] * matrix.m[9];

  inv.m[12] =  -matrix.m[4]  * matrix.m[9] * matrix.m[14] +
                matrix.m[4]  * matrix.m[10] * matrix.m[13] +
                matrix.m[8]  * matrix.m[5] * matrix.m[14] -
                matrix.m[8]  * matrix.m[6] * matrix.m[13] -
                matrix.m[12] * matrix.m[5] * matrix.m[10] +
                matrix.m[12] * matrix.m[6] * matrix.m[9];

  inv.m[1] = -matrix.m[1]  * matrix.m[10] * matrix.m[15] +
              matrix.m[1]  * matrix.m[11] * matrix.m[14] +
              matrix.m[9]  * matrix.m[2] * matrix.m[15] -
              matrix.m[9]  * matrix.m[3] * matrix.m[14] -
              matrix.m[13] * matrix.m[2] * matrix.m[11] +
              matrix.m[13] * matrix.m[3] * matrix.m[10];

  inv.m[5] =  matrix.m[0]  * matrix.m[10] * matrix.m[15] -
              matrix.m[0]  * matrix.m[11] * matrix.m[14] -
              matrix.m[8]  * matrix.m[2] * matrix.m[15] +
              matrix.m[8]  * matrix.m[3] * matrix.m[14] +
              matrix.m[12] * matrix.m[2] * matrix.m[11] -
              matrix.m[12] * matrix.m[3] * matrix.m[10];

  inv.m[9] = -matrix.m[0]  * matrix.m[9] * matrix.m[15] +
              matrix.m[0]  * matrix.m[11] * matrix.m[13] +
              matrix.m[8]  * matrix.m[1] * matrix.m[15] -
              matrix.m[8]  * matrix.m[3] * matrix.m[13] -
              matrix.m[12] * matrix.m[1] * matrix.m[11] +
              matrix.m[12] * matrix.m[3] * matrix.m[9];

  inv.m[13] = matrix.m[0]  * matrix.m[9] * matrix.m[14] -
              matrix.m[0]  * matrix.m[10] * matrix.m[13] -
              matrix.m[8]  * matrix.m[1] * matrix.m[14] +
              matrix.m[8]  * matrix.m[2] * matrix.m[13] +
              matrix.m[12] * matrix.m[1] * matrix.m[10] -
              matrix.m[12] * matrix.m[2] * matrix.m[9];

  inv.m[2] =  matrix.m[1]  * matrix.m[6] * matrix.m[15] -
              matrix.m[1]  * matrix.m[7] * matrix.m[14] -
              matrix.m[5]  * matrix.m[2] * matrix.m[15] +
              matrix.m[5]  * matrix.m[3] * matrix.m[14] +
              matrix.m[13] * matrix.m[2] * matrix.m[7] -
              matrix.m[13] * matrix.m[3] * matrix.m[6];

  inv.m[6] = -matrix.m[0]  * matrix.m[6] * matrix.m[15] +
              matrix.m[0]  * matrix.m[7] * matrix.m[14] +
              matrix.m[4]  * matrix.m[2] * matrix.m[15] -
              matrix.m[4]  * matrix.m[3] * matrix.m[14] -
              matrix.m[12] * matrix.m[2] * matrix.m[7] +
              matrix.m[12] * matrix.m[3] * matrix.m[6];

  inv.m[10] = matrix.m[0]  * matrix.m[5] * matrix.m[15] -
              matrix.m[0]  * matrix.m[7] * matrix.m[13] -
              matrix.m[4]  * matrix.m[1] * matrix.m[15] +
              matrix.m[4]  * matrix.m[3] * matrix.m[13] +
              matrix.m[12] * matrix.m[1] * matrix.m[7] -
              matrix.m[12] * matrix.m[3] * matrix.m[5];

  inv.m[14] =  -matrix.m[0]  * matrix.m[5] * matrix.m[14] +
                matrix.m[0]  * matrix.m[6] * matrix.m[13] +
                matrix.m[4]  * matrix.m[1] * matrix.m[14] -
                matrix.m[4]  * matrix.m[2] * matrix.m[13] -
                matrix.m[12] * matrix.m[1] * matrix.m[6] +
                matrix.m[12] * matrix.m[2] * matrix.m[5];

  inv.m[3] = -matrix.m[1] * matrix.m[6] * matrix.m[11] +
              matrix.m[1] * matrix.m[7] * matrix.m[10] +
              matrix.m[5] * matrix.m[2] * matrix.m[11] -
              matrix.m[5] * matrix.m[3] * matrix.m[10] -
              matrix.m[9] * matrix.m[2] * matrix.m[7] +
              matrix.m[9] * matrix.m[3] * matrix.m[6];

  inv.m[7] =  matrix.m[0] * matrix.m[6] * matrix.m[11] -
              matrix.m[0] * matrix.m[7] * matrix.m[10] -
              matrix.m[4] * matrix.m[2] * matrix.m[11] +
              matrix.m[4] * matrix.m[3] * matrix.m[10] +
              matrix.m[8] * matrix.m[2] * matrix.m[7] -
              matrix.m[8] * matrix.m[3] * matrix.m[6];

  inv.m[11] =  -matrix.m[0] * matrix.m[5] * matrix.m[11] +
                matrix.m[0] * matrix.m[7] * matrix.m[9] +
                matrix.m[4] * matrix.m[1] * matrix.m[11] -
                matrix.m[4] * matrix.m[3] * matrix.m[9] -
                matrix.m[8] * matrix.m[1] * matrix.m[7] +
                matrix.m[8] * matrix.m[3] * matrix.m[5];

  inv.m[15] = matrix.m[0] * matrix.m[5] * matrix.m[10] -
              matrix.m[0] * matrix.m[6] * matrix.m[9] -
              matrix.m[4] * matrix.m[1] * matrix.m[10] +
              matrix.m[4] * matrix.m[2] * matrix.m[9] +
              matrix.m[8] * matrix.m[1] * matrix.m[6] -
              matrix.m[8] * matrix.m[2] * matrix.m[5];

  det = matrix.m[0] * inv.m[0] + matrix.m[1] * inv.m[4] + matrix.m[2] * inv.m[8] + matrix.m[3] * inv.m[12];

  det = 1.0 / det;

  for (i = 0; i < 16; i++)
        inv.m[i] = inv.m[i] * det;

  return inv;
}

#endif
