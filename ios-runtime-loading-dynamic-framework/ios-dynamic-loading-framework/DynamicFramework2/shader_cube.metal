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

struct InVertex {
        float   vid [[attribute(0)]];
};

struct ColorInOut {
        float4 position [[position]];
        float2 texcoord;
        uint    texid;
        float4 color;
};

struct DirLight {
        packed_float3 direction;
        packed_float3 ambient;
        packed_float3 diffuse;
        packed_float3 specular;
};

struct PointLight {
        packed_float3 position;
        float _constant;
        float linear;
        float quadratic;
        packed_float3 ambient;
        packed_float3 diffuse;
        packed_float3 specular;
};

struct SpotLight {
        packed_float3 position;
        packed_float3 direction;
        float cut_off;
        float outer_cut_off;
        float _constant;
        float linear;
        float quadratic;
        packed_float3 ambient;
        packed_float3 diffuse;
        packed_float3 specular;
};

struct shader_3d_color_uniform {
        float4x4 proj;
        float4x4 view;
        packed_float3 view_position;
        DirLight dlights[3];
        PointLight plights[3];
        SpotLight slights[1];
} __attribute__((aligned (1024)));


static float4 decodeFloatColor(int val)
{
        /*
         * value for each channel decoded from val has range [-127, 127] so
         * we need add 128 to it
         */
        float4 c;
        /*
         * val / 65536     : shift right val by 16 bit to clear green and blue channel and get red channel
         */
//        c[0] = float(val / 65536 + 128);
        c[0] = float((val >> 16) + 128);
        /*
         * val * 65536     : shift left val by 16 bit to clear red channel
         * (x) / 16777216  : shift right x by 24 bit to clear blue channel and get green channel
         */
//        c[1] = float((val * 65536) / 16777216 + 128);
        c[1] = float(((val << 16) >> 24) + 128);
        /*
         * val * 16777216  : shift left val by 24 bit to clear red and green channel
         * (x) / 16777216  : shift right x by 24 bit to get blue channel
         */
        //        c[2] = float((val * 16777216) / 16777216 + 128);
        c[2] = float(((val << 24) >> 24) + 128);
        /*
         * currently we fill alpha channel by max value 255 because input value
         * present only RGB color
         * currently I can only map RGB to 32-bit integer, alpha channel is challenging
         */
        c[3] = 255.0;
        /*
         * convert to normalized color
         */
        c /= 255.0;
        return c;
}

static float2 decodeTexcoord(float val)
{
    float2 tc;
    tc[0] = float(int(val) << 16) / 10000.0;
    tc[1] = float(((int(val) << 16) >> 16 ) / 10000.0;
    return tc;
}

static float4x4 matrix4_inverse(float4x4 m)
{
        float
                a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
                a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
                a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
                a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

                b00 = a00 * a11 - a01 * a10,
                b01 = a00 * a12 - a02 * a10,
                b02 = a00 * a13 - a03 * a10,
                b03 = a01 * a12 - a02 * a11,
                b04 = a01 * a13 - a03 * a11,
                b05 = a02 * a13 - a03 * a12,
                b06 = a20 * a31 - a21 * a30,
                b07 = a20 * a32 - a22 * a30,
                b08 = a20 * a33 - a23 * a30,
                b09 = a21 * a32 - a22 * a31,
                b10 = a21 * a33 - a23 * a31,
                b11 = a22 * a33 - a23 * a32,

                det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

        return float4x4(
                float4(a11 * b11 - a12 * b10 + a13 * b09,
                a02 * b10 - a01 * b11 - a03 * b09,
                a31 * b05 - a32 * b04 + a33 * b03,
                a22 * b04 - a21 * b05 - a23 * b03) / det,
                float4(a12 * b08 - a10 * b11 - a13 * b07,
                a00 * b11 - a02 * b08 + a03 * b07,
                a32 * b02 - a30 * b05 - a33 * b01,
                a20 * b05 - a22 * b02 + a23 * b01) / det,
                float4(a10 * b10 - a11 * b08 + a13 * b06,
                a01 * b08 - a00 * b10 - a03 * b06,
                a30 * b04 - a31 * b02 + a33 * b00,
                a21 * b02 - a20 * b04 - a23 * b00) / det,
                float4(a11 * b07 - a10 * b09 - a12 * b06,
                a00 * b09 - a01 * b07 + a02 * b06,
                a31 * b01 - a30 * b03 - a32 * b00,
                a20 * b03 - a21 * b01 + a22 * b00) / det
        );
}

static float4x4 matrix4_transpose(float4x4 matrix)
{
        return float4x4(
                float4(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]),
                float4(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]),
                float4(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]),
                float4(matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]));
}

static float3 CalcDirLight(DirLight light, float3 normal, float bright)
{
   float3 lightDir = normalize(-light.direction);
   float diff = fmax(dot(normal, lightDir), 0);
   return light.ambient + light.diffuse * diff * bright;
}

vertex ColorInOut vertex_3d_color(constant InVertex *vertex_array [[ buffer(0) ]],
                              constant float4x4 *trans[[buffer(1)]],
                              constant float *brights[[buffer(2)]],
                              constant packed_float3 *vertex_1[[buffer(3)]],
                              constant packed_float3 *vertex_2[[buffer(4)]],
                              constant packed_float3 *vertex_3[[buffer(5)]],
                              constant packed_float3 *normal_1[[buffer(6)]],
                              constant packed_float3 *normal_2[[buffer(7)]],
                              constant packed_float3 *normal_3[[buffer(8)]],
                              constant packed_float4 *vertex_color[[buffer(9)]],
                              constant packed_float4 *texcoords[[buffer(10)]],
                              constant uint *divisor[[buffer(11)]],
                              constant shader_3d_color_uniform &uniform [[buffer(12)]],
                              ushort vid [[vertex_id]],
                              uint iid [[instance_id]]
                              )
{
    ColorInOut out;
    /*
     * parse input
     */
    float4x4 proj               = uniform.proj;
    float4x4 view               = uniform.view;

    float4x4 transform          = trans[iid / divisor[1]];
    float bright                = brights[iid / divisor[2]];

    constant packed_float3 *vertice[3]   = {vertex_1, vertex_2, vertex_3};
    constant packed_float3 *normals[3]   = {normal_1, normal_2, normal_3};

    float2 texcs[3] = {
        decodeTexcoord(texcoords[iid / divisor[10]][0]),
        decodeTexcoord(texcoords[iid / divisor[10]][1]),
        decodeTexcoord(texcoords[iid / divisor[10]][2])
    };


    float3 vertex_i             = vertice[vid][iid / divisor[3 + vid]];
    float3 normal_i             = normals[vid][iid / divisor[6 + vid]];

    float4 alpha                = decodeFloatColor(vertex_color[iid / divisor[9]][3]);
    float4 decodeColor          = decodeFloatColor(vertex_color[iid / divisor[9]][vid]);
    decodeColor.a               = alpha[vid];

    /*
     * calculate out
     */
    out.position                = proj * view * transform * float4(vertex_i, 1.0);
    float4x4 inv                = matrix4_transpose(matrix4_inverse(transform));
    float3 pixel_normal         = float3x3(float3(inv[0].xyz), float3(inv[1].xyz), float3(inv[2].xyz)) * normal_i;
    float3 norm                 = normalize(pixel_normal);
    float3 result               = float3(0);

    result += CalcDirLight(uniform.dlights[0], norm, bright);

    out.color = decodeColor * float4(result, 1.0);
    out.texid = uint(texcoords[iid / divisor[10]][3]);
    out.texcoord = texcs[vid];

    return out;
}


// static float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir) {
//     float3 lightDir = normalize(light.position - fragPos);
//     // Diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // Attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light._constant + light.linear * distance + light.quadratic * (distance * distance));
//
//     float3 ambient = float3(light.ambient * attenuation);
//     float3 diffuse = float3(light.diffuse * diff * attenuation);
//
//     return (ambient + diffuse);
// }

fragment float4 fragment_3d_color(ColorInOut in [[stage_in]],
                                  constant shader_3d_color_uniform &uniform [[buffer(0)]],
                                  texture2d<float, access::sample> texture0 [[texture(0)]],
                                  texture2d<float, access::sample> texture1 [[texture(1)]],
                                  texture2d<float, access::sample> texture2 [[texture(2)]],
                                  texture2d<float, access::sample> texture3 [[texture(3)]],
                                  texture2d<float, access::sample> texture4 [[texture(4)]],
                                  texture2d<float, access::sample> texture5 [[texture(5)]],
                                  texture2d<float, access::sample> texture6 [[texture(6)]],
                                  texture2d<float, access::sample> texture7 [[texture(7)]],
                                  sampler texSampler [[sampler(0)]]
                             )
{
    float4 pixel;

    if(in.texid >= 7) {
        pixel = texture7.sample(texSampler, in.texcoord);
    } else if(in.texid >= 6) {
        pixel = texture6.sample(texSampler, in.texcoord);
    } else if(in.texid >= 5) {
        pixel = texture5.sample(texSampler, in.texcoord);
    } else if(in.texid >= 4) {
        pixel = texture4.sample(texSampler, in.texcoord);
    } else if(in.texid >= 3) {
        pixel = texture3.sample(texSampler, in.texcoord);
    } else if(in.texid >= 2) {
        pixel = texture2.sample(texSampler, in.texcoord);
    } else if(in.texid >= 1) {
        pixel = texture1.sample(texSampler, in.texcoord);
    } else if(in.texid >= 0) {
        pixel = texture0.sample(texSampler, in.texcoord);
    }

    return in.color * pixel;
};
