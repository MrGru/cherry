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

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct ColorInOut {
        float4 position [[position]];
        float2 texcoord;
        float4 color;
};

struct camera_uniform {
    float4x4        project;
    float4x4        view;
    packed_float3   position;
};

struct transform_uniform {
    float4x4        transform;
};

vertex ColorInOut vertex_2d_texture_color(
                              device packed_float2 *position[[buffer(0)]],
                              device packed_float2 *texcoord[[buffer(1)]],
                              device packed_float4 *color[[buffer(2)]],
                              constant camera_uniform &cam [[buffer(3)]],
                              constant transform_uniform &tran [[buffer(4)]],
                              ushort vid [[vertex_id]]
                              )
{
    ColorInOut out;
    out.position = cam.project * cam.view * tran.transform * float4(float2(position[vid]), 0.0, 1.0);
    out.texcoord = float2(texcoord[vid]);
    out.texcoord.y = 1.0 - out.texcoord.y;
    out.color    = float4(color[vid]);
    return out;
}


fragment float4 fragment_2d_texture_color(ColorInOut in [[stage_in]],
                                  constant camera_uniform &cam [[buffer(0)]],
                                  constant transform_uniform &tran [[buffer(1)]],
                                  texture2d<float> texture0 [[texture(0)]],
                                  sampler texSampler0 [[sampler(0)]]
                             )
{
    return in.color * texture0.sample(texSampler0, in.texcoord);
};
