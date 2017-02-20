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
#ifndef __CHERRY_STRING_H__
#define __CHERRY_STRING_H__

#include <cherry/types.h>
#include <string.h>

struct string *string_alloc(size_t len);

struct string *string_alloc_chars(char *s, size_t len);

void string_free(struct string *p);

void string_reserve(struct string *p, size_t size);

void string_cat(struct string *p, char *src, size_t src_len);

static inline void string_cat_string(struct string *p, struct string *q)
{
        string_cat(p, q->ptr, q->len);
}

void string_cat_int(struct string *p, int number);

void string_replace(struct string *p, char *search, char *replace);

u8 string_contain(struct string *p, char *search);

#endif
