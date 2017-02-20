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
#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/graphic/shader/descriptor_collection.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/string.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/bytes.h>

static struct map       *groups = NULL;

static void clear()
{
        map_deep_free(groups, struct shader *, shader_free);
}

#if GFX == OGL

static inline struct string *get_vert()
{
        return file_read_string("res/shaders/shader_3d_color.vert");
}

static inline struct string *get_frag()
{
        return file_read_string("res/shaders/shader_3d_color.frag");
}

#elif GFX == MTL

static inline struct string *get_vert()
{
        return NULL;
}

static inline struct string *get_frag()
{
        return NULL;
}

#endif

struct shader *shader_3d_color_alloc(u8 direction_lights, u8 point_lights, u8 spot_lights)
{
        if(!groups) {
                groups = map_alloc(sizeof(struct shader *));
                cache_add(clear);
        }

        struct bytes *bytes = bytes_alloc();
        bytes_cat(bytes, &direction_lights, sizeof(direction_lights));
        bytes_cat(bytes, &point_lights, sizeof(point_lights));
        bytes_cat(bytes, &spot_lights, sizeof(spot_lights));

        struct shader *instance = map_get(groups, struct shader *, bytes->ptr, bytes->len);

        if(instance) {
                return instance;
                bytes_free(bytes);
        }

        struct shader_descriptor *des = descriptor_3d_color_get(1);

        /* vert */
        struct string *vert = get_vert();
        /* frag */
        struct string *frag = get_frag();
        /*
         * replace number lights definitions
         */
        {
                char s[] = "NR_DIRECTION_LIGHTS __";
                snprintf(s + sizeof(s) - 3, 3, "%02d", direction_lights);
                string_replace(frag, "NR_DIRECTION_LIGHTS __", s);
        }
        {
                char s[] = "NR_POINT_LIGHTS __";
                snprintf(s + sizeof(s) - 3, 3, "%02d", point_lights);
                string_replace(frag, "NR_POINT_LIGHTS __", s);
        }
        {
                char s[] = "NR_SPOT_LIGHTS __";
                snprintf(s + sizeof(s) - 3, 3, "%02d", spot_lights);
                string_replace(frag, "NR_SPOT_LIGHTS __", s);
        }

        instance = shader_alloc(vert->ptr, frag->ptr, des);
        map_set(groups, bytes->ptr, bytes->len, &instance);

        string_free(vert);
        string_free(frag);

        struct shader_3d_color_uniform scu;

        shader_reserve_uniform(instance, SHADER_3D_COLOR_PROJECT, UNIFORM_M4,
                "project", (void*)&scu.project - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_VIEW, UNIFORM_M4,
                "view", (void*)&scu.view - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_VIEW_POSITION, UNIFORM_F3,
                "view_position", (void*)&scu.view_position - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_0_DIRECTION, UNIFORM_F3,
                "dirLights[0].direction", (void*)&scu.dlights[0].direction - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_0_AMBIENT, UNIFORM_F3,
                "dirLights[0].ambient", (void*)&scu.dlights[0].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_0_DIFFUSE, UNIFORM_F3,
                "dirLights[0].diffuse", (void*)&scu.dlights[0].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_0_SPECULAR, UNIFORM_F3,
                "dirLights[0].specular", (void*)&scu.dlights[0].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_1_DIRECTION, UNIFORM_F3,
                "dirLights[1].direction", (void*)&scu.dlights[1].direction - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_1_AMBIENT, UNIFORM_F3,
                "dirLights[1].ambient", (void*)&scu.dlights[1].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_1_DIFFUSE, UNIFORM_F3,
                "dirLights[1].diffuse", (void*)&scu.dlights[1].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_1_SPECULAR, UNIFORM_F3,
                "dirLights[1].specular", (void*)&scu.dlights[1].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_2_DIRECTION, UNIFORM_F3,
                "dirLights[2].direction", (void*)&scu.dlights[2].direction - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_2_AMBIENT, UNIFORM_F3,
                "dirLights[2].ambient", (void*)&scu.dlights[2].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_2_DIFFUSE, UNIFORM_F3,
                "dirLights[2].diffuse", (void*)&scu.dlights[2].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_DIR_LIGHT_2_SPECULAR, UNIFORM_F3,
                "dirLights[2].specular", (void*)&scu.dlights[2].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_POSITION, UNIFORM_F3,
                "pointLights[0].position", (void*)&scu.plights[0].position - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_CONSTANT, UNIFORM_F1,
                "pointLights[0].constant", (void*)&scu.plights[0].constant - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_LINEAR, UNIFORM_F1,
                "pointLights[0].linear", (void*)&scu.plights[0].linear - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_QUADRATIC, UNIFORM_F1,
                "pointLights[0].quadratic", (void*)&scu.plights[0].quadratic - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_AMBIENT, UNIFORM_F3,
                "pointLights[0].ambient", (void*)&scu.plights[0].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_DIFFUSE, UNIFORM_F3,
                "pointLights[0].diffuse", (void*)&scu.plights[0].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_0_SPECULAR, UNIFORM_F3,
                "pointLights[0].specular", (void*)&scu.plights[0].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_POSITION, UNIFORM_F3,
                "pointLights[1].position", (void*)&scu.plights[1].position - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_CONSTANT, UNIFORM_F1,
                "pointLights[1].constant", (void*)&scu.plights[1].constant - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_LINEAR, UNIFORM_F1,
                "pointLights[1].linear", (void*)&scu.plights[1].linear - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_QUADRATIC, UNIFORM_F1,
                "pointLights[1].quadratic", (void*)&scu.plights[1].quadratic - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_AMBIENT, UNIFORM_F3,
                "pointLights[1].ambient", (void*)&scu.plights[1].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_DIFFUSE, UNIFORM_F3,
                "pointLights[1].diffuse", (void*)&scu.plights[1].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_1_SPECULAR, UNIFORM_F3,
                "pointLights[1].specular", (void*)&scu.plights[1].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_POSITION, UNIFORM_F3,
                "pointLights[2].position", (void*)&scu.plights[2].position - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_CONSTANT, UNIFORM_F1,
                "pointLights[2].constant", (void*)&scu.plights[2].constant - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_LINEAR, UNIFORM_F1,
                "pointLights[2].linear", (void*)&scu.plights[2].linear - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_QUADRATIC, UNIFORM_F1,
                "pointLights[2].quadratic", (void*)&scu.plights[2].quadratic - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_AMBIENT, UNIFORM_F3,
                "pointLights[2].ambient", (void*)&scu.plights[2].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_DIFFUSE, UNIFORM_F3,
                "pointLights[2].diffuse", (void*)&scu.plights[2].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_POINT_LIGHT_2_SPECULAR, UNIFORM_F3,
                "pointLights[2].specular", (void*)&scu.plights[2].specular - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_POSITION, UNIFORM_F3,
                "spotLights[0].position", (void*)&scu.slights[0].position - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_DIRECTION, UNIFORM_F3,
                "spotLights[0].direction", (void*)&scu.slights[0].direction - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_CUT_OFF, UNIFORM_F1,
                "spotLights[0].cut_off", (void*)&scu.slights[0].cut_off - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_OUTER_CUT_OFF, UNIFORM_F1,
                "spotLights[0].outer_cut_off", (void*)&scu.slights[0].outer_cut_off - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_CONSTANT, UNIFORM_F1,
                "spotLights[0].constant", (void*)&scu.slights[0].constant - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_LINEAR, UNIFORM_F1,
                "spotLights[0].linear", (void*)&scu.slights[0].linear - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_QUADRATIC, UNIFORM_F1,
                "spotLights[0].quadratic", (void*)&scu.slights[0].quadratic- (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_AMBIENT, UNIFORM_F3,
                "spotLights[0].ambient", (void*)&scu.slights[0].ambient - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_DIFFUSE, UNIFORM_F3,
                "spotLights[0].diffuse", (void*)&scu.slights[0].diffuse - (void*)&scu);
        shader_reserve_uniform(instance, SHADER_3D_COLOR_SPOT_LIGHT_0_SPECULAR, UNIFORM_F3,
                "spotLights[0].specular", (void*)&scu.slights[0].specular - (void*)&scu);

#if GFX == MTL
        u8 i;
        struct shader_texture_uniform u;
        for_i(i, BUFFERS) {
                instance->uniforms[i] = device_buffer_alloc(BUFFER_VERTICE, 0, BUFFER_PINNED);
                device_buffer_fill(instance->uniforms[i], &u, sizeof(u));
        }
#endif
        bytes_free(bytes);
        return instance;
}
