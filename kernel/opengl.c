#include <cherry/graphic/graphic.h>

#if TARGET_RENDERER == RENDERER_OPENGL

int opengl_max_textures         = 8;
int depth_texture_enable        = 1;
int video_width                 = 800;
int video_height                = 480;
int shadow_width                = 1024;

#include <cherry/memory.h>
#include <cherry/graphic/device_buffer.h>

struct device_buffer_head *device_buffer_alloc()
{
        struct device_buffer_head *head = smalloc(sizeof(struct device_buffer_head));
        glGenBuffers(1, &head->id);
        return head;
}

void device_buffer_fill(struct device_buffer_head *head, void *bytes, uint64_t size)
{
        glBufferData(GL_ARRAY_BUFFER, size, bytes, GL_STATIC_DRAW);
}

void device_buffer_sub(struct device_buffer_head *head, uint64_t offset, void *bytes, uint64_t size)
{
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, bytes);
}

void device_buffer_free(struct device_buffer_head *head)
{
        glDeleteBuffers(1, &head->id);
        sfree(head);
}

#endif
