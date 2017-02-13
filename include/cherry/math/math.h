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
#ifndef __CHERRY_MATH_MATH_H__
#define __CHERRY_MATH_MATH_H__

#include <cherry/math/types.h>
#include <cherry/math/vec2.h>
#include <cherry/math/vec3.h>
#include <cherry/math/vec4.h>
#include <cherry/math/mat3.h>
#include <cherry/math/mat4.h>
#include <cherry/math/quat.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(X) ((X) * M_PI / 180)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG(X) ((X) * 180 / M_PI)
#endif

#ifndef EPSILON
#define EPSILON 0.0000000016
#endif

#endif
