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
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/memory.h>
#include <cherry/math/types.h>

struct device_buffer *buffer_transform_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union mat4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union mat4), location);
        device_buffer_fill(buffer, data, sizeof(union mat4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_color_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec4), location);
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_vertex_color_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec3) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec3), location);
        device_buffer_fill(buffer, data, sizeof(union vec3) * instances);
        sfree(data);
        return buffer;
}

/*
 * 3d buffers collection
 */
static float triangle[3] = {
        0.01,
        1.01,
        2.01
};
struct device_buffer *buffer_triangle_alloc(u8 location)
{
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, 0, location);
        device_buffer_fill(buffer, triangle, sizeof(triangle));
        return buffer;
}

struct device_buffer *buffer_3d_vertex_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec3) * instances);
        smemset(data, 0, sizeof(union vec3) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec3), location);
        device_buffer_fill(buffer, data, sizeof(union vec3) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_3d_normal_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec3) * instances);
        smemset(data, 0, sizeof(union vec3) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec3), location);
        device_buffer_fill(buffer, data, sizeof(union vec3) * instances);
        sfree(data);
        return buffer;
}

/*
 * 2d buffers collection
 */
static float quad[6] = {
        0.01,
        1.01,
        2.01,

        3.01,
        4.01,
        5.01
};

struct device_buffer *buffer_quad_alloc(u8 location)
{
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, 0, location);
        device_buffer_fill(buffer, quad, sizeof(quad));
        return buffer;
}

struct device_buffer *buffer_vertex_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec4), location);
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_texcoord_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(union vec4), location);
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_z_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(float), location);
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_texid_alloc(u32 instances, u8 location)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE,
                sizeof(float), location);
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}
