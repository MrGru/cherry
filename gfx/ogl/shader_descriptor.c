#include <cherry/graphic/shader.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/string.h>

/*
 * allocate new shader attribute decsriptor
 */
struct shader_attribute_descriptor *__shader_attribute_descriptor_alloc(u8 type, u16 offset, char *name)
{
        struct shader_attribute_descriptor *p = smalloc(sizeof(struct shader_attribute_descriptor));
        p->type         = type;
        p->offset       = offset;
        p->name         = string_alloc_chars(name, strlen(name));
        return p;
}

/*
 * deallocate shader attribute descriptor
 */
void shader_attribute_descriptor_free(struct shader_attribute_descriptor *p)
{
        string_free(p->name);
        sfree(p);
}

#endif
