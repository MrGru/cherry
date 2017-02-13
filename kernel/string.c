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
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>

struct string *string_alloc(size_t len)
{
        struct string *s = smalloc(sizeof(struct string));
        s->len = 0;
        s->ptr = smalloc(len + 1);
        s->ptr[0] = '\0';
        return s;
};

struct string *string_alloc_chars(char *s, size_t len)
{
        struct string *p = string_alloc(len);
        string_cat(p, s, len);
        return p;
}

void string_free(struct string *p)
{
        sfree(p->ptr);
        sfree(p);
}

void string_reserve(struct string *p, size_t size)
{
        p->ptr = srealloc(p->ptr, size + 1);
}

void string_cat(struct string *p, char *src, size_t src_len)
{
        string_reserve(p, p->len + src_len);
        smemcpy(p->ptr + p->len, src, src_len);
        p->len += src_len;
        p->ptr[p->len] = '\0';
}

void string_cat_int(struct string *p, int number)
{
        int_to_chars(v, number)
        string_cat(p, v, strlen(v));
}
