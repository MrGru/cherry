#include <cherry/graphic/graphic.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/device_buffer.h>

struct device_buffer *device_buffer_alloc()
{
        struct device_buffer *p = smalloc(sizeof(struct device_buffer));
        glGenBuffers(1, &p->id);
        return p;
}

void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size)
{
        glBindBuffer(GL_ARRAY_BUFFER, p->id);
        glBufferData(GL_ARRAY_BUFFER, size, bytes, GL_STATIC_DRAW);
}

void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size)
{
        glBindBuffer(GL_ARRAY_BUFFER, p->id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, bytes);
}

void device_buffer_free(struct device_buffer *p)
{
        glDeleteBuffers(1, &p->id);
        sfree(p);
}

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc()
{
        struct device_buffer_group *p = smalloc(sizeof(struct device_buffer_group));
        p->buffers = array_alloc(sizeof(struct device_buffer *), ORDERED);
        p->pipeline = NULL;
        glGenVertexArrays(1, &p->id);
        return p;
}

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p)
{
        glDeleteVertexArrays(1, &p->id);
        array_free(p->buffers);
        sfree(p);
}

#endif
