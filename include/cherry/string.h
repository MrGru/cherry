/*
 * string definition
 */
#ifndef __CHERRY_STRING_H__
#define __CHERRY_STRING_H__

#include <cherry/memory.h>
#include <string.h>

struct string_head {
        unsigned long len;
};

static inline char *string_cat(char *, char *, size_t);

static inline char *string_alloc(size_t len)
{
        struct string_head *h = smalloc(sizeof(struct string_head) + len + 1);
        h->len = 0;
        char *r = (char *)(h + 1);
        *r = '\0';
        return r;
};

static inline char *string_alloc_chars(char *src, size_t len)
{
        char *p = string_alloc(len);
        return string_cat(p, src, len);
}

static inline void string_free(char *p)
{
        struct string_head *h = (struct string_head *)p - 1;
        sfree(h);
}

static inline char *string_realloc(char *p, size_t size)
{
        struct string_head *h = (struct string_head *)p - 1;
        h = srealloc(h, sizeof(struct string_head) + size + 1);
        return (char*)(h + 1);
}

static inline size_t string_len(char *p)
{
        struct string_head *h = (struct string_head *)p - 1;
        return h->len;
}

static inline char *string_cat(char *p, char *src, size_t src_len)
{
        struct string_head *h = (struct string_head *)p - 1;
        h = srealloc(h, sizeof(struct string_head) + h->len + src_len + 1);
        size_t h_len = h->len;
        h->len += src_len;
        p = (char *)(h + 1);
        memcpy(p + h_len, src, src_len);
        p[h->len] = '\0';
        return p;
}

#endif
