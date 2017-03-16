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

union vec4 convert_to_3d_texcoord(union vec2 uv1, union vec2 uv2, union vec2 uv3, int texid)
{
        union vec4 v;
        /*
         * store coord.ts by rule : t is stored in the first 16 bit, s is stored in the last 16 bit
         */
        v.x     = ((int)(uv1.x * 10000) << 16) + (int)(uv1.y * 10000);
        v.y     = ((int)(uv2.x * 10000) << 16) + (int)(uv2.y * 10000);
        v.z     = ((int)(uv3.x * 10000) << 16) + (int)(uv3.y * 10000);
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

void decode_3d_texcoord(union vec4 texcoord, union vec2 *result)
{
        result[0] = __decode_texcoord((int)texcoord.v[0]);
        result[1] = __decode_texcoord((int)texcoord.v[1]);
        result[2] = __decode_texcoord((int)texcoord.v[2]);
}
