#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/memory.h>
#include <cherry/math/types.h>

static float quad[6] = {
        0.01,
        1.01,
        2.01,

        3.01,
        4.01,
        5.01
};

// static float quad[24] = {
//         0.01,
//         1.01,
//         2.01,
//
//         3.01,
//         4.01,
//         5.01
// };

struct device_buffer *buffer_quad_alloc()
{
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, 0);
        device_buffer_fill(buffer, quad, sizeof(quad));
        return buffer;
}

struct device_buffer *buffer_vertex_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union vec4));
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_texcoord_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union vec4));
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_quad_texroot_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec2) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union vec2));
        device_buffer_fill(buffer, data, sizeof(union vec2) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_quad_texrange_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec2) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union vec2));
        device_buffer_fill(buffer, data, sizeof(union vec2) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_z_alloc(u16 instances)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(float));
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_texid_alloc(u16 instances)
{
        void *data = smalloc(sizeof(float) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(float));
        device_buffer_fill(buffer, data, sizeof(float) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_transform_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union mat4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union mat4));
        device_buffer_fill(buffer, data, sizeof(union mat4) * instances);
        sfree(data);
        return buffer;
}

struct device_buffer *buffer_color_alloc(u16 instances)
{
        void *data = smalloc(sizeof(union vec4) * instances);
        struct device_buffer *buffer = device_buffer_alloc(BUFFER_VERTICE, sizeof(union vec4));
        device_buffer_fill(buffer, data, sizeof(union vec4) * instances);
        sfree(data);
        return buffer;
}
