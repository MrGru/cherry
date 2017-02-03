#include <cherry/string.h>

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
