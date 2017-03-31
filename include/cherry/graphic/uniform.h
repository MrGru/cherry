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
#ifndef __CHERRY_GRAPHIC_UNIFORM_H__
#define __CHERRY_GRAPHIC_UNIFORM_H__

#include <cherry/graphic/types.h>

struct uniform_buffer *camera_uniform_buffer_alloc();

struct uniform_buffer *transform_uniform_buffer_alloc();

struct uniform_buffer *light_uniform_buffer_alloc();

void uniform_buffer_init(struct uniform_buffer *p, size_t size);

void uniform_buffer_set(struct uniform_buffer *p, u8 index, void *data, size_t len);

#if GFX == OGL
void uniform_buffer_force_update(struct uniform_buffer *p, struct link_uniform *link);
#endif

#if GFX == MTL
void uniform_buffer_update(struct uniform_buffer *p, u8 frame);
#endif

void uniform_buffer_free(struct uniform_buffer *p);

#endif
