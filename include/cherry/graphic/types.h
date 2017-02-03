/*
 * graphic types definition
 */
#ifndef __CHERRY_GRAPHIC_TYPES_H__
#define __CHERRY_GRAPHIC_TYPES_H__

#include <cherry/graphic/graphic.h>
#include <cherry/list.h>

struct device_buffer_head {
        struct list_head head;

#if TARGET_RENDERER == RENDERER_OPENGL
        uint32_t id; /* reference to device buffer */
#elif TARGET_RENDERER == RENDERER_METAL
        void *ptr; /* reference to device buffer */
        uint64_t size;
#endif
};

struct device_buffer_array_head {
        struct list_head head;
#if TARGET_RENDERER == RENDERER_OPENGL
        uint32_t id;
#endif
};

#endif
