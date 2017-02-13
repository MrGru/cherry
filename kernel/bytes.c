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
#include <cherry/bytes.h>
#include <cherry/memory.h>

struct bytes *bytes_alloc()
{
        struct bytes *p = smalloc(sizeof(struct bytes));
        p->len = 0;
        p->ptr = NULL;
        return p;
}

void bytes_free(struct bytes *p)
{
        if(p->ptr) sfree(p->ptr);
        sfree(p);
}

void bytes_reserve(struct bytes *p, u32 size)
{
        p->ptr = srealloc(p->ptr, size);
}

void bytes_cat(struct bytes *p, void *s, u32 size)
{
        bytes_reserve(p, p->len + size);
        smemcpy(p->ptr + p->len, s, size);
        p->len += size;
}
