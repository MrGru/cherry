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

static struct shader *instance = NULL;

static void clear()
{
        if(instance) {
                shader_free(instance);
                instance = NULL;
        }
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

struct shader *shader_3d_color_alloc()
{
        if(!instance) {
                cache_add(clear);
        } else {
                return instance;
        }
        struct shader_descriptor *des = descriptor_3d_color_get(2);

        /* vert */
        struct string *vert = get_vert();
        /* frag */
        struct string *frag = get_frag();
        instance = shader_alloc(vert->ptr, frag->ptr, des);
        string_free(vert);
        string_free(frag);

        shader_reserve_uniform(instance, SHADER_3D_COLOR_PROJECT, UNIFORM_M4,
                "project", offsetof(struct shader_3d_color_uniform, project));
        shader_reserve_uniform(instance, SHADER_3D_COLOR_VIEW, UNIFORM_M4,
                "view", offsetof(struct shader_3d_color_uniform, view));

#if GFX == MTL
        u8 i;
        struct shader_texture_uniform u;
        for_i(i, BUFFERS) {
                instance->uniforms[i] = device_buffer_alloc(BUFFER_VERTICE, 0);
                device_buffer_fill(instance->uniforms[i], &u, sizeof(u));
        }
#endif

        return instance;
}
