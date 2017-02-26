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
        precision highp float;
        #define input           varying
        #define get_pixel       texture2D
        #define out_pixel       gl_FragColor
#else
        #define input           in
        #define get_pixel       texture
        out vec4                fragColor;
        #define out_pixel       fragColor
#endif

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
 * pixel_color used to adjust fragment color
 */
input vec4      pixel_color;
input vec3      pixel_normal;
input vec3      pixel_frag_pos;

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

vec3 CalcDirLight(DirLight ulight, vec3 normal, vec3 viewDir)
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

void main()
{
        vec3 norm       = normalize(pixel_normal);
        vec3 viewDir    = normalize(view_position - pixel_frag_pos);

        vec3 result     = vec3(0.0, 0.0, 0.0);

/*
 * update direction lights
 */
#if NR_DIRECTION_LIGHTS > 0
        result += CalcDirLight(dirLights[0], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 1
        result += CalcDirLight(dirLights[1], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 2
        result += CalcDirLight(dirLights[2], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 3
        result += CalcDirLight(dirLights[3], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 4
        result += CalcDirLight(dirLights[4], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 5
        result += CalcDirLight(dirLights[5], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 6
        result += CalcDirLight(dirLights[6], norm, viewDir);
#endif

#if NR_DIRECTION_LIGHTS > 7
        result += CalcDirLight(dirLights[7], norm, viewDir);
#endif

/*
 * update point lights
 */

#if NR_POINT_LIGHTS > 0
        result += CalcPointLight(pointLights[0], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 1
        result += CalcPointLight(pointLights[1], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 2
        result += CalcPointLight(pointLights[2], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 3
        result += CalcPointLight(pointLights[3], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 4
        result += CalcPointLight(pointLights[4], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 5
        result += CalcPointLight(pointLights[5], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 6
        result += CalcPointLight(pointLights[6], norm, pixel_frag_pos, viewDir);
#endif

#if NR_POINT_LIGHTS > 7
        result += CalcPointLight(pointLights[7], norm, pixel_frag_pos, viewDir);
#endif

/*
 * update spot lights
 */
#if NR_SPOT_LIGHTS != 0
        for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        {
            result += CalcSpotLight(spotLights[i], norm, pixel_frag_pos, viewDir);
        }
#endif

/*
 * export result
 */
#if NR_DIRECTION_LIGHTS != 0 || NR_POINT_LIGHTS != 0 || NR_SPOT_LIGHTS != 0
        out_pixel = pixel_color * vec4(result, 1.0);
#else
        out_pixel = pixel_color;
#endif
}
