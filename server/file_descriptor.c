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
#include <cherry/server/file_descriptor.h>
#include <cherry/memory.h>
#include <cherry/array.h>

/*
 * append more 16 blocks or 1024 slots
 */
static inline void __append_set(struct file_descriptor_set *p)
{
        array_reserve(p->set, p->set->len + 16);
        int i;
        for_i(i, 16) {
                u64 block = 0;
                array_push(p->set, &block);
        }
}

static inline void __check_set_size(struct file_descriptor_set *p, u32 fd)
{
        fd /= 64;
        while (fd >= p->set->len) {
                __append_set(p);
        }
}

struct file_descriptor_set *file_descriptor_set_alloc(u32 max)
{
        struct file_descriptor_set *p   = smalloc(sizeof(struct file_descriptor_set));
        p->set                          = array_alloc(sizeof(u64), ORDERED);
        __check_set_size(p, max);
        return p;
}

void file_descriptor_set_free(struct file_descriptor_set *p)
{
        array_free(p->set);
        sfree(p);
}

void file_descriptor_set_add(struct file_descriptor_set *p, u32 fd)
{
        __check_set_size(p, fd);
        u16 block       = fd / 64;
        u16 bit         = fd % 64;
        u64 *data       = (u64 *)p->set->ptr;
        data[block]     |= ((u64)1 << bit);
}

void file_descriptor_set_remove(struct file_descriptor_set *p, u32 fd)
{
        u16 count = fd / 64;
        if(count < p->set->len) {
                u16 block       = fd / 64;
                u16 bit         = fd % 64;
                u64 *data       = (u64 *)p->set->ptr;
                data[block]     &= ~((u64)1 << bit);
        }
}

void file_descriptor_set_clean(struct file_descriptor_set *p)
{
        u64 *data;
        array_for_each(data, p->set) {
                *data = (u64)0;
        }
}

void file_descriptor_set_assign(struct file_descriptor_set *p, struct file_descriptor_set *q)
{
        file_descriptor_set_clean(p);

        u64 *data;
        u16 index;
        array_for_each_index(data, index, q->set) {
                if(index >= p->set->len) {
                        __append_set(p);
                }
                array_set(p->set, index, data);
        }
}

void file_descriptor_set_get_active(struct file_descriptor_set *p, struct array *o)
{
        u64 *data;
        u32 fd;
        u16 i;
        array_for_each_index(data, i, p->set) {
                fd      = i * 64;
                u64 b   = *data;
                u64 pb  = b;
                while(b != 0) {
                        fd      += __builtin_ctzl(b);
                        array_push(o, &fd);
                        b       >>= (__builtin_ctzl(b)) + 1;
                        ++fd;
                        /*
                         * if b is created from 1 << 63 then shift right 64 bit will not affect anything
                         */
                        if(b == pb) break;
                }
        }
}
