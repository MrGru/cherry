/*
 * math super header
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
