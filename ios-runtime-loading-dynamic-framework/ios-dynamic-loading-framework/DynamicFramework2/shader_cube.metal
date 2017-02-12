/*
 Copyright (C) 2015 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 lighting shader for Basic Metal 3D
 */

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// variables in constant address space

typedef struct
{
    packed_float3 position [[attribute(0)]];
    packed_float3 normal [[attribute(1)]];
} vertex_t;

struct ColorInOut {
    float4 position [[position]];
    float3 fragPos;
    float3 normal;
    float4 color;
};

struct colorInstance
{
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

struct shader_cube_uniform {
    float4x4 proj;
    float4x4 view;
    packed_float3 view_position;
    DirLight dlights[3];
    PointLight plights[3];
    SpotLight slights[1];
} __attribute__((aligned (1024)));


// vertex shader function
vertex ColorInOut vertex_cube(constant vertex_t* vertex_array [[ buffer(0) ]],
                              constant float4x4 *trans[[buffer(1)]],
                              constant colorInstance *colors[[buffer(2)]],
                              constant shader_cube_uniform &uniform [[buffer(3)]],
                              ushort vid [[vertex_id]]
                              ,
                              ushort iid [[instance_id]]
                              )
{
    ColorInOut out;
    
    float4x4 model = trans[iid];
    float4 color = colors[iid].color;
    float4x4 proj = uniform.proj;
    float4x4 view = uniform.view;

    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    out.position = proj * view * model * in_position;


    out.fragPos = float3(model * in_position);
    out.normal = float3(vertex_array[vid].normal);
    out.color = color;
    
    return out;
}

//static half3 CalcDirLight(AAPL::DirLight light, float3 normal, float3 viewDir)
//{
//    float3 lightDir = normalize(-light.direction);
//    float diff = fmax(dot(normal, lightDir), 0);
//    return half3(light.ambient + light.diffuse * diff);
//}

static float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir) {
    float3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light._constant + light.linear * distance + light.quadratic * (distance * distance));

    float3 ambient = float3(light.ambient * attenuation);
    float3 diffuse = float3(light.diffuse * diff * attenuation);

    return (ambient + diffuse);
}

// fragment shader function
fragment float4 fragment_cube(ColorInOut in [[stage_in]]
                             ,
                             constant shader_cube_uniform &uniform [[buffer(0)]]
                             )
{
    float3 norm = normalize(in.normal);
    float3 viewDir = normalize(uniform.view_position - in.fragPos);
    float3 result = float3(0, 0, 0);
    result += CalcPointLight(uniform.plights[0], norm, in.fragPos, viewDir);
    
    return in.color * float4(result, 1.0);
};

