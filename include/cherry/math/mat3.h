/*
 * 3-dimensions matrix implementaion
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
