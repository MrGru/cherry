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

void bytes_reserve(struct bytes *p, uint32_t size)
{
        p->ptr = srealloc(p->ptr, size);
}

void bytes_cat(struct bytes *p, void *s, uint32_t size)
{
        bytes_reserve(p, p->len + size);
        smemcpy(p->ptr + p->len, s, size);
        p->len += size;
}
