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

static inline GLenum device_buffer_location(struct device_buffer *p)
{
        GLenum target;
        switch (p->location) {
                case BUFFER_PINNED:
                        target = GL_STATIC_DRAW;
                        break;
                case BUFFER_SHARED:
                        target = GL_DYNAMIC_DRAW;
                        break;
        }
        return target;
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

void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size)
{
        GLenum target = device_buffer_target(p);
        glBindBuffer(target, p->id);
        glBufferData(target, size, bytes, device_buffer_location(p));
}

void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size)
{
        GLenum target = device_buffer_target(p);
        glBindBuffer(target, p->id);
        glBufferSubData(target, offset, size, bytes);
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
