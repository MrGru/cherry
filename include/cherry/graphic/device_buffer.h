/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __CHERRY_GRAPHIC_DEVICE_BUFFER_H__
#define __CHERRY_GRAPHIC_DEVICE_BUFFER_H__

#include <cherry/graphic/types.h>

/*
 * allocate new device buffer
 *
 * location should be BUFFER_PINNED if buffer is not changed
 * or changed a little time during it's life time
 *
 * location should be BUFFER_SHARED if buffer is changed almost
 * every frame
 *
 * BUFFER_PINNED may take longer time to update data but gpu can access
 * buffer directly to draw
 * BUFFER_SHARED may take less time to update data but gpu has to use DMA
 * to access buffer to draw
 */
struct device_buffer *device_buffer_alloc(u8 type, u16 item_size, u8 location);

#if GFX == OGL
GLenum device_buffer_target(struct device_buffer *p);
#endif

/*
 * resize buffer and fill size bytes
 * @p           : buffer to resize/file
 * @bytes       : bytes array
 * @size        : bytes array length
 */
void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size);

/*
 * @p           : buffer to change
 * @offset      : index that buffer changes from
 * @bytes       : bytes array
 * @size        : bytes array length
 */
void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size);

/*
 * deallocate device buffer
 */
void device_buffer_free(struct device_buffer *p);

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc();

/*
 * retain buffer b and push it to group g
 */
void device_buffer_group_add(struct device_buffer_group *g, struct device_buffer *b);

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p);

#endif
