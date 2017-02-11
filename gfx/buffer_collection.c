#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/memory.h>
#include <cherry/math/types.h>

static float quad[18] = {
        -1,  1,  0,
        -1, -1,  0,
         1, -1,  0,

        -1,  1,  0,
         1,  1,  0,
         1, -1,  0
};

struct device_buffer *buffer_quad_alloc()
{
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, quad, sizeof(quad));
        return buffer;
}

static float quad_coord[12] = {
        0, 0,
        0, 1,
        1, 1,

        0, 0,
        1, 0,
        1, 1
};

struct device_buffer *buffer_quad_texcoord_alloc()
{
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, quad_coord, sizeof(quad_coord));
        return buffer;
}

struct device_buffer *buffer_quad_texroot_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec2) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(union vec2) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_quad_texrange_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec2) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(union vec2) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_z_alloc(u16 instances)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_texid_alloc(u16 instances)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_transform_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union mat4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(union mat4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_color_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE);
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}
