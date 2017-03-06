/*
 * Copyright (C) 2017 Manh Tran
 *
 * quick approach for allocation and free memory
 * single thread - need implementation for concurrent
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
#version 100

#ifdef GL_ES
        #define input   attribute
        #define output  varying
#else
        #define input   in
        #define output  out
#endif

/*
 * z used to sort instances by depth test function after fragment step
 * vid used to access current texcoord, vertex
 */
input float     vid;
input vec4      color;
input mat4      transform;
input vec3      vertex_1;
input vec3      vertex_2;
input vec3      vertex_3;
input vec3      normal_1;
input vec3      normal_2;
input vec3      normal_3;
input vec4      vertex_color;

/*
 * pixel_color used to adjust fragment color
 */
output vec4     pixel_color;

uniform mat4    project;
uniform mat4    view;

/*
 * We have to replace __ by number of lights we want to use
 */
#define NR_DIRECTION_LIGHTS __
#define NR_POINT_LIGHTS __
#define NR_SPOT_LIGHTS __

struct DirLight {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
};

struct PointLight {
        vec3 position;

        float constant;
        float linear;
        float quadratic;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
};

struct SpotLight {
        vec3 position;
        vec3 direction;
        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
};

/*
 * declare light uniforms
 */
#if NR_DIRECTION_LIGHTS != 0
uniform DirLight        dirLights[NR_DIRECTION_LIGHTS];
#endif

#if NR_POINT_LIGHTS != 0
uniform PointLight      pointLights[NR_POINT_LIGHTS];
#endif

#if NR_SPOT_LIGHTS != 0
uniform SpotLight       spotLights[NR_SPOT_LIGHTS];
#endif

/*
 * we need camera position
 */
uniform vec3    view_position;

vec3 CalcDirLight(DirLight ulight, vec3 normal)
{
        vec3 color      = vec3(1.0, 1.0, 1.0);
        vec3 lightDir   = normalize(-ulight.direction);

        float diff      = max(dot(normal, lightDir), 0.0);

        vec3 ambient    = ulight.ambient;
        vec3 diffuse    = ulight.diffuse * diff;
        return (ambient + diffuse);
}

vec3 CalcPointLight(PointLight ulight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
        vec3 lightDir           = normalize(ulight.position - fragPos);

        float diff              = max(dot(normal, lightDir), 0.0);

        vec3 reflectDir         = reflect(-lightDir, normal);
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
        float spec              = max(dot(viewDir, reflectDir), 0.0);

        float distance          = length(ulight.position - fragPos);
        float attenuation       = 1.0 / (ulight.constant + ulight.linear * distance + ulight.quadratic * (distance * distance));

        vec3 ambient            = ulight.ambient * attenuation;
        vec3 diffuse            = ulight.diffuse * diff * attenuation;
        vec3 specular           = ulight.specular * spec;

        return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight ulight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
        vec3 lightDir           = normalize(ulight.position - fragPos);

        float diff              = max(dot(normal, lightDir), 0.0);

        float distance          = length(ulight.position - fragPos);
        float attenuation       = 1.0 / (ulight.constant + ulight.linear * distance + ulight.quadratic * (distance * distance));

        float theta             = dot(lightDir, normalize(-ulight.direction));
        float epsilon           = ulight.cutOff - ulight.outerCutOff;
        float intensity                 = clamp((theta - ulight.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 ambient            = ulight.ambient;
        vec3 diffuse            = ulight.diffuse * diff;

        ambient                 *= attenuation * intensity;
        diffuse                 *= attenuation * intensity;
        return (ambient + diffuse);
}


/*
 * see @pack_rgb_to_float in gfx/graphic.c for more information
 */
vec4 decodeFloatColor(highp int val)
{
        /*
         * value for each channel decoded from val has range [-127, 127] so
         * we need add 128 to it
         */
        vec4 c;
        /*
         * val / 65536     : shift right val by 16 bit to clear green and blue channel and get red channel
         */
        c.r = float(val / 65536 + 128);
        /*
         * val * 65536     : shift left val by 16 bit to clear red channel
         * (x) / 16777216  : shift right x by 24 bit to clear blue channel and get green channel
         */
        c.g = float((val * 65536) / 16777216 + 128);
        /*
         * val * 16777216  : shift left val by 24 bit to clear red and green channel
         * (x) / 16777216  : shift right x by 24 bit to get blue channel
         */
        c.b = float((val * 16777216) / 16777216 + 128);
        /*
         * currently we fill alpha channel by max value 255 because input value
         * present only RGB color
         * currently I can only map RGB to 32-bit integer, alpha channel is challenging
         */
        c.a = 255.0;
        /*
         * convert to normalized color
         */
        c /= 255.0;
        return c;
}

mat4 matrix4_inverse(mat4 m)
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

        return mat4(
                a11 * b11 - a12 * b10 + a13 * b09,
                a02 * b10 - a01 * b11 - a03 * b09,
                a31 * b05 - a32 * b04 + a33 * b03,
                a22 * b04 - a21 * b05 - a23 * b03,
                a12 * b08 - a10 * b11 - a13 * b07,
                a00 * b11 - a02 * b08 + a03 * b07,
                a32 * b02 - a30 * b05 - a33 * b01,
                a20 * b05 - a22 * b02 + a23 * b01,
                a10 * b10 - a11 * b08 + a13 * b06,
                a01 * b08 - a00 * b10 - a03 * b06,
                a30 * b04 - a31 * b02 + a33 * b00,
                a21 * b02 - a20 * b04 - a23 * b00,
                a11 * b07 - a10 * b09 - a12 * b06,
                a00 * b09 - a01 * b07 + a02 * b06,
                a31 * b01 - a30 * b03 - a32 * b00,
                a20 * b03 - a21 * b01 + a22 * b00
        ) / det;
}


mat4 matrix4_transpose(mat4 matrix)
{
        mat4 m = mat4(matrix[0].x, matrix[1].x, matrix[2].x, matrix[3].x,
                matrix[0].y, matrix[1].y, matrix[2].y, matrix[3].y,
                matrix[0].z, matrix[1].z, matrix[2].z, matrix[3].z,
                matrix[0].w, matrix[1].w, matrix[2].w, matrix[3].w);
        return m;
}

/*
 * WebGL currently does not support array index as attribute so I need use branch
 */
void main()
{
        vec4 alpha              = decodeFloatColor(int(vertex_color[3]));
        vec4 decodecolor;
        if(vid >= 2.0) {
                decodecolor        = decodeFloatColor(int(vertex_color[2]));
                decodecolor.a           = alpha[2];
        } else if(vid >= 1.0) {
                decodecolor        = decodeFloatColor(int(vertex_color[1]));
                decodecolor.a           = alpha[1];
        } else {
                decodecolor        = decodeFloatColor(int(vertex_color[0]));
                decodecolor.a           = alpha[0];
        }

        /*
         * working for 2d rendering so pos.z = 0
         */
        vec3 pos;
        if(vid >= 2.0) {
                pos                = vertex_3;
        } else if(vid >= 1.0) {
                pos                = vertex_2;
        } else {
                pos                = vertex_1;
        }
        gl_Position             = project * view * transform * vec4(pos, 1.0);

        /*
         * pass pixel parameters
         */
        vec3 pixel_normal;
        if(vid >= 2.0) {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_3;
        } else if(vid >= 1.0) {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_2;
        } else {
                pixel_normal       = mat3(matrix4_transpose(matrix4_inverse(transform))) * normal_1;
        }

        vec3 norm               = normalize(pixel_normal);

        vec3 result             = vec3(0.0);

#if NR_DIRECTION_LIGHTS >= 1
        result                  += CalcDirLight(dirLights[0], norm);
#endif
#if NR_POINT_LIGHTS >= 1
        vec3 pixel_frag_pos     = vec3(transform * vec4(pos, 1.0));
        vec3 viewDir            = normalize(view_position - pixel_frag_pos);
        result                  += CalcPointLight(pointLights[0], norm, pixel_frag_pos, viewDir);
#endif

        pixel_color             = color * decodecolor * vec4(result, 1.0);
}
