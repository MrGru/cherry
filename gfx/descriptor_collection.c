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
#include <cherry/graphic/shader/descriptor_collection.h>
#include <cherry/memory.h>
#include <cherry/graphic/shader.h>
#include <cherry/array.h>
#include <cherry/map.h>

static struct shader_descriptor *descriptor_2d_texture = NULL;

static void __descriptor_2d_texture_free()
{
        shader_descriptor_free(descriptor_2d_texture);
}

struct shader_descriptor *descriptor_2d_texture_get()
{
        if( ! descriptor_2d_texture) {
                cache_add(__descriptor_2d_texture_free);
        } else {
                return descriptor_2d_texture;
        }

        struct shader_descriptor *des = shader_descriptor_alloc();
        descriptor_2d_texture = des;
        /* vertex id */
        struct shader_buffer_descriptor *sbd = shader_buffer_descriptor_alloc(sizeof(float), 0, 0);
        struct shader_attribute_descriptor *sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "vid");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /*z*/
        sbd = shader_buffer_descriptor_alloc(sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "z");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* transform */
        sbd = shader_buffer_descriptor_alloc(16 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_MAT4, 0, "transform");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* color */
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, "color");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* texid */
        sbd = shader_buffer_descriptor_alloc(sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "texid");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* texcoords */
#define ADD_TEXCOORDS(name)                                                     \
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);          \
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, name);       \
        array_push(sbd->attributes, &sad);                                      \
        array_push(des->buffers, &sbd);

        ADD_TEXCOORDS("texcoord_1")
        ADD_TEXCOORDS("texcoord_2")
        ADD_TEXCOORDS("texcoord_3")

#undef ADD_TEXCOORDS
        /* vertex */
#define ADD_VERTEX(name)                                                        \
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);          \
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, name);       \
        array_push(sbd->attributes, &sad);                                      \
        array_push(des->buffers, &sbd);

        ADD_VERTEX("vertex_1")
        ADD_VERTEX("vertex_2")
        ADD_VERTEX("vertex_3")

#undef ADD_VERTEX

        return descriptor_2d_texture;
}

static struct map *descriptor_3d_color_map = NULL;

static void __descriptor_3d_color_free()
{
        map_deep_free(descriptor_3d_color_map, struct shader_descriptor *, shader_descriptor_free);
}

struct shader_descriptor *descriptor_3d_color_get(u32 triangles_per_object)
{
        if( ! descriptor_3d_color_map) {
                descriptor_3d_color_map = map_alloc(sizeof(struct shader_descriptor *));
                cache_add(__descriptor_3d_color_free);
        } else {
                struct shader_descriptor *p = map_get(descriptor_3d_color_map, struct shader_descriptor *, qpkey(triangles_per_object));
                if(p) return p;
        }

        struct shader_descriptor *des = shader_descriptor_alloc();
        map_set(descriptor_3d_color_map, qpkey(triangles_per_object), &des);
        /* vertex id */
        struct shader_buffer_descriptor *sbd = shader_buffer_descriptor_alloc(sizeof(float), 0, 0);
        struct shader_attribute_descriptor *sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "vid");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* transform */
        sbd = shader_buffer_descriptor_alloc(16 * sizeof(float), 1, triangles_per_object);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_MAT4, 0, "transform");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* bright */
        sbd = shader_buffer_descriptor_alloc(sizeof(float), 1, triangles_per_object);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "bright");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* vertex */
#define ADD_VERTEX(name)                                                        \
        sbd = shader_buffer_descriptor_alloc(3 * sizeof(float), 1, 1);          \
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC3, 0, name);       \
        array_push(sbd->attributes, &sad);                                      \
        array_push(des->buffers, &sbd);

        ADD_VERTEX("vertex_1")
        ADD_VERTEX("vertex_2")
        ADD_VERTEX("vertex_3")

#undef ADD_VERTEX
#define ADD_NORMAL(name)                                                        \
        sbd = shader_buffer_descriptor_alloc(3 * sizeof(float), 1, 1);          \
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC3, 0, name);       \
        array_push(sbd->attributes, &sad);                                      \
        array_push(des->buffers, &sbd);

        ADD_NORMAL("normal_1")
        ADD_NORMAL("normal_2")
        ADD_NORMAL("normal_3")

#undef ADD_NORMAL
        /* vertex color */
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, "vertex_color");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* texcoord */
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, "texcoord");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        return des;
}
