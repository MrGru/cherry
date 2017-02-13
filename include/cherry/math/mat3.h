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
#ifndef __CHERRY_MATH_MAT3_H__
#define __CHERRY_MATH_MAT3_H__

#include <cherry/math/types.h>
#include <math.h>

extern union mat3 mat3_identity;

static inline union mat3 mat3(float *m)
{
        return (union mat3){
                m[0], m[1], m[2],
                m[3], m[4], m[5],
                m[6], m[7], m[8]
        };
}

static inline union mat3 mat3_transpose(float *m)
{
        return (union mat3){
                m[0], m[3], m[6],
                m[1], m[4], m[7],
                m[2], m[5], m[8]
        };
}

#endif
