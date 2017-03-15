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
#include <cherry/graphic/graphic.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/stdio.h>

u32 current_bind_buffer = 0;

GLenum device_buffer_target(struct device_buffer *p)
{
        GLenum target;
        switch(p->type) {
                case BUFFER_VERTICE:
                        target = GL_ARRAY_BUFFER;
                        break;
                case BUFFER_ELEMENT:
                        target = GL_ELEMENT_ARRAY_BUFFER;
                        break;
        }
        return target;
}

/*
 * in OpenGL
 * @GL_STATIC_DRAW means memory is allocated in device memory (vram)
 * @GL_DYNAMIC_DRAW means memory is allocated in host memory (ram)
 */
static inline GLenum device_buffer_location(struct device_buffer *p)
{
        GLenum target;
        switch (p->location) {
                case BUFFER_DEVICE:
                        target = GL_STATIC_DRAW;
                        break;
                case BUFFER_PINNED:
                        target = GL_DYNAMIC_DRAW;
                        break;
        }
        return target;
}

static inline void __device_buffer_bind(struct device_buffer *p)
{
        if(p->id != current_bind_buffer) {
                current_bind_buffer = p->id;
                GLenum target = device_buffer_target(p);
                glBindBuffer(target, p->id);
        }
}


struct device_buffer *device_buffer_alloc(u8 type, u16 item_size, u8 location)
{
        struct device_buffer *p = smalloc(sizeof(struct device_buffer));
        glGenBuffers(1, &p->id);
        p->ref                  = 0;
        p->type                 = type;
        p->item_size            = item_size;
        p->location             = location;
        return p;
}

void device_buffer_bind(struct device_buffer *p)
{
        __device_buffer_bind(p);
}

void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size)
{
        GLenum target = device_buffer_target(p);
        __device_buffer_bind(p);
        glBufferData(target, size, bytes, device_buffer_location(p));
}

/*
 * @GL_MAP_UNSYNCHRONIZED_BIT is used because when I developing Cherry I use
 * triple bufferings so I think no need to apply a fence for updating buffer data
 *
 * I think glMapBufferRange is best performance in most cases especially
 * when device buffer's memory is allocated in RAM (buffer type is @BUFFER_PINNED)
 *
 * If there are some problems then try to increase BUFFERS (cherry/graphic/graphic.h)
 * to higher or implement client/server fence due to memory bandwith
 */
void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size)
{
#if OS == WEB
        /*
         * currently I can't compile WebGL with glMapBufferRange
         */
        GLenum target = device_buffer_target(p);
        __device_buffer_bind(p);
        glBufferSubData(target, offset, size, bytes);
#else
        GLenum target = device_buffer_target(p);
        __device_buffer_bind(p);
        switch (p->location) {
                case BUFFER_DEVICE:
                        glBufferSubData(target, offset, size, bytes);
                        break;

                case BUFFER_PINNED: {
                                void *ptr = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT
                                        | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
                                smemcpy(ptr, bytes, size);
                                glUnmapBuffer(target);
                        }
                        break;

                default:
                        break;
        }
#endif
}

void device_buffer_free(struct device_buffer *p)
{
        p->ref--;
        if(p->ref <= 0) {
                glDeleteBuffers(1, &p->id);
                sfree(p);
        }
}

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc()
{
        struct device_buffer_group *p = smalloc(sizeof(struct device_buffer_group));
        p->buffers = array_alloc(sizeof(struct device_buffer *), ORDERED);
        glGenVertexArrays(1, &p->id);
        return p;
}

void device_buffer_group_add(struct device_buffer_group *g, struct device_buffer *b)
{
        b->ref++;
        array_push(g->buffers, &b);
}


void device_buffer_group_bind_construct(struct device_buffer_group *p)
{
        current_bind_buffer = 0;
        glBindVertexArray(p ? p->id : 0);
}

void device_buffer_group_bind_draw(struct device_buffer_group *p)
{
        glBindVertexArray(p ? p->id : 0);
}

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p)
{
        glDeleteVertexArrays(1, &p->id);
        array_deep_free(p->buffers, struct device_buffer *, device_buffer_free);
        sfree(p);
}

#endif
