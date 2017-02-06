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
