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
#ifndef __CHERRY_GRAPHIC_BUFFER_BUFFER_COLLECTION_H__
#define __CHERRY_GRAPHIC_BUFFER_BUFFER_COLLECTION_H__

/*
 * location should be BUFFER_DEVICE if buffer is not changed
 * or changed a little time during it's life time
 *
 * location should be BUFFER_PINNED if buffer is changed almost
 * every frame
 *
 * BUFFER_DEVICE may take longer time to update data but gpu can access
 * buffer directly to draw
 * BUFFER_PINNED may take less time to update data but gpu has to use DMA
 * to access buffer to draw
 */
#include <cherry/graphic/buffer/types.h>

struct device_buffer *buffer_transform_alloc(u32 instances, u8 location);

struct device_buffer *buffer_color_alloc(u32 instances, u8 location);

struct device_buffer *buffer_vertex_color_alloc(u32 instances, u8 location);

struct device_buffer *buffer_bright_alloc(u32 instances, u8 location);

/*
 * 3d buffers collection
 */
struct device_buffer *buffer_triangle_alloc(u8 location);

struct device_buffer *buffer_3d_vertex_alloc(u32 instances, u8 location);

struct device_buffer *buffer_3d_normal_alloc(u32 instances, u8 location);

/*
 * 2d buffers collection
 */
struct device_buffer *buffer_quad_alloc(u8 location);

struct device_buffer *buffer_vertex_alloc(u32 instances, u8 location);

struct device_buffer *buffer_texcoord_alloc(u32 instances, u8 location);

struct device_buffer *buffer_z_alloc(u32 instances, u8 location);

struct device_buffer *buffer_texid_alloc(u32 instances, u8 location);

#endif
