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
        return file_read_string("res/shaders/shader_texture.vert");
}

static inline struct string *get_frag()
{
        return file_read_string("res/shaders/shader_texture.frag");
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

struct shader *shader_texture_alloc()
{
        if(!instance) {
                cache_add(clear);
        } else {
                return instance;
        }
        struct shader_descriptor *des = descriptor_2d_texture_get();

        /* vert */
        struct string *vert = get_vert();
        /* frag */
        struct string *frag = get_frag();
        instance = shader_alloc(vert->ptr, frag->ptr, des);
        string_free(vert);
        string_free(frag);

        shader_reserve_uniform(instance, SHADER_TEXTURE_PROJECT, UNIFORM_M4,
                "project", offsetof(struct shader_texture_uniform, project));
        shader_reserve_uniform(instance, SHADER_TEXTURE_VIEW, UNIFORM_M4,
                "view", offsetof(struct shader_texture_uniform, view));

#if GFX == MTL
        u8 i;
        struct shader_texture_uniform u;
        for_i(i, BUFFERS) {
                instance->uniforms[i] = device_buffer_alloc(BUFFER_VERTICE, 0, BUFFER_PINNED);
                device_buffer_fill(instance->uniforms[i], &u, sizeof(u));
        }
#endif

#if GFX == OGL
        u8 i;
        for_i_from(i, SHADER_TEXTURE_IMAGE_0, SHADER_TEXTURE_IMAGE_N) {
                struct string *name = string_alloc(10);
                string_cat(name, "image[", sizeof("image[") - 1);
                string_cat_int(name, i - SHADER_TEXTURE_IMAGE_0);
                string_cat(name, "]", sizeof("]") - 1);
                shader_reserve_uniform(instance, i, UNIFORM_I1, name->ptr, 0);

                struct shader_uniform *u = shader_uniform_alloc();
                shader_set_uniform(instance, i, u);
                u->ref++;
                array_push(instance->texture_uniforms, &u);

                u32 id = 0;
                shader_uniform_update(u, &id, sizeof(id));

                string_free(name);
        }
#endif

        return instance;
}
