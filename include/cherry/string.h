/*
 * string definition
 */
#ifndef __CHERRY_STRING_H__
#define __CHERRY_STRING_H__

#include <cherry/memory.h>
#include <cherry/types.h>
#include <string.h>

struct string *string_alloc(size_t len);

struct string *string_alloc_chars(char *s, size_t len);

void string_free(struct string *p);

void string_reserve(struct string *p, size_t size);

void string_cat(struct string *p, char *src, size_t src_len);

static inline size_t string_len(struct string *p)
{
        return p->len;
}

#endif
