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
#ifndef __CHERRY_GRAPHIC_SHADER_DESCRIPTOR_COLLECTION_H__
#define __CHERRY_GRAPHIC_SHADER_DESCRIPTOR_COLLECTION_H__

#include <cherry/graphic/shader/types.h>

struct shader_descriptor *descriptor_2d_texture_get();

struct shader_descriptor *descriptor_3d_color_get(u32 triangles_per_object);

#endif
