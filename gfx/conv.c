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
#include <cherry/graphic/node/conv.h>
#include <cherry/stdio.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>

union texcoord_3d convert_to_3d_texcoord(union vec2 uv1, union vec2 uv2, union vec2 uv3, int texid)
{
        union texcoord_3d v;
        /*
         * store coord.ts by rule : t is stored in the first 16 bit, s is stored in the last 16 bit
         */
        v.x     = ((u32)(uv1.x * 10000) << 16) + (u32)(uv1.y * 10000);
        v.y     = ((u32)(uv2.x * 10000) << 16) + (u32)(uv2.y * 10000);
        v.z     = ((u32)(uv3.x * 10000) << 16) + (u32)(uv3.y * 10000);

        /*
         * GPU has some trouble if we cast from (float)1 to (int)1
         * One solution is cast from (float)1.01 to (int)1. It works.
         */
        v.w     = texid + 0.01;
        return v;
}

static inline union vec2 __decode_texcoord(int val) {
        union vec2 v;
        v.x = (float)(val >> 16) / 10000.0;
        v.y = (float)((val << 16) >> 16) / 10000.0;

        return v;
}

void decode_3d_texcoord(union texcoord_3d texcoord, union vec2 *result)
{
        result[0] = __decode_texcoord(texcoord.v[0]);
        result[1] = __decode_texcoord(texcoord.v[1]);
        result[2] = __decode_texcoord(texcoord.v[2]);
}
