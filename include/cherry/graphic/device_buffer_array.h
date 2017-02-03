#ifndef __CHERRY_GRAPHIC_DEVICE_BUFFER_ARRAY_H__
#define __CHERRY_GRAPHIC_DEVICE_BUFFER_ARRAY_H__

#include <cherry/graphic/types.h>

static inline void INIT_DEVICE_BUFFER_ARRAY_HEAD(struct device_buffer_array_head *head)
{
        INIT_LIST_HEAD(&head->head);
#if TARGET_RENDERER == RENDERER_OPENGL
        glGenVertexArrays(1, &head->id);
#endif
}

static inline void FREE_DEVICE_BUFFER_ARRAY_HEAD(struct device_buffer_array_head *head)
{
#if TARGET_RENDERER == RENDERER_OPENGL
        glDeleteVertexArrays(1, &head->id);
#endif
}

#endif
