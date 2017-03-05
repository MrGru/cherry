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
#include <cherry/graphic/shader.h>

#if GFX == MTL
#include <cherry/memory.h>

/*
 * allocate new shader attribute decsriptor
 */
struct shader_attribute_descriptor *__shader_attribute_descriptor_alloc(u8 type, u16 offset)
{
        struct shader_attribute_descriptor *p = smalloc(sizeof(struct shader_attribute_descriptor));
        p->type   = type;
        p->offset = offset;
        return p;
}

/*
 * deallocate shader attribute descriptor
 */
void shader_attribute_descriptor_free(struct shader_attribute_descriptor *p)
{
        sfree(p);
}

#endif
