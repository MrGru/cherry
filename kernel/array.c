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
#include <cherry/array.h>
#include <cherry/memory.h>
#include <string.h>

static inline void __array_reserve(struct array *p, u16 len)
{
        p->ptr = srealloc(p->ptr, len * p->item_size);
}

struct array *array_alloc(u16 item_size, u8 ordered)
{
        struct array *p = smalloc(sizeof(struct array));
        p->ptr = NULL;
        p->end = 0;
        p->len = 0;
        p->item_size = item_size;
        p->ordered = ordered;
        return p;
}

void array_free(struct array *p)
{
        if(p->ptr) sfree(p->ptr);
        sfree(p);
}

void array_reserve(struct array *p, u16 len)
{
        p->ptr = srealloc(p->ptr, len * p->item_size);
}

void array_force_len(struct array *p, u16 len)
{
        p->ptr = srealloc(p->ptr, len * p->item_size);
        p->len = len;
        p->end = (size_t)p->ptr + p->item_size * p->len;
}

void array_push(struct array *p, void *d)
{
        __array_reserve(p, p->len + 1);
        smemcpy(p->ptr + p->len * p->item_size, d, p->item_size);
        p->len += 1;
        p->end = (size_t)p->ptr + p->item_size * p->len;
}

void array_set(struct array *p, u16 index, void *d)
{
        smemcpy(p->ptr + p->item_size * index, d, p->item_size);
}

void array_copy(struct array *p, u16 index, void *o)
{
        smemcpy(o, p->ptr + p->item_size * index, p->item_size);
}

void array_zero(struct array *p)
{
        if(p->ptr) smemset(p->ptr, 0, p->len * p->item_size);
}

void array_remove(struct array *p, u16 index)
{
        if(index >= p->len) return;
        u16 item_size = p->item_size;

        if(index != p->len - 1) {
                if(p->ordered == ORDERED) {
                        char* x = p->ptr + index * item_size;
                        char* y = p->ptr + (index + 1) * item_size;
                        unsigned long s = (p->len - index - 1) * item_size;
                        smemcpy(x, y, s);
                } else if(index < p->len - 1) {
                        smemcpy(p->ptr + index * item_size, p->ptr + (p->len - 1) * item_size, item_size);
                }
        }
        p->len--;
        p->end = (size_t)p->ptr + p->item_size * p->len;
}
