#include <cherry/graphic/shader.h>
#include <cherry/memory.h>
#include <cherry/array.h>

/*
 * allocate new shader buffer descriptor
 */
struct shader_buffer_descriptor *shader_buffer_descriptor_alloc(u16 vertex_size, u8 instanced, u8 divisor)
{
        struct shader_buffer_descriptor *p = smalloc(sizeof(struct shader_buffer_descriptor));
        p->attributes   = array_alloc(sizeof(struct shader_attribute_descriptor *), ORDERED);
        p->vertex_size  = vertex_size;
        p->instanced    = instanced;
        p->divisor      = divisor;
        return p;
}

/*
 * deallocate shader buffer descriptor
 */
void shader_buffer_descriptor_free(struct shader_buffer_descriptor *p)
{
        array_deep_free(p->attributes, struct shader_attribute_descriptor *, shader_attribute_descriptor_free);
        sfree(p);
}

/*
 * allocate new shader descriptor
 */
struct shader_descriptor *shader_descriptor_alloc()
{
        struct shader_descriptor *p = smalloc(sizeof(struct shader_descriptor));
        p->buffers = array_alloc(sizeof(struct shader_buffer_descriptor *), ORDERED);
        return p;
}

/*
 * deallocate shader descriptor
 */
void shader_descriptor_free(struct shader_descriptor *p)
{
        array_deep_free(p->buffers, struct shader_buffer_descriptor *, shader_buffer_descriptor_free);
        sfree(p);
}
