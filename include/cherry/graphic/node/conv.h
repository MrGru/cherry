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
#ifndef __CHERRY_GRAPHIC_NODE_CONV_H__
#define __CHERRY_GRAPHIC_NODE_CONV_H__

#include <cherry/graphic/node/types.h>

union texcoord_3d convert_to_3d_texcoord(union vec2 uv1, union vec2 uv2, union vec2 uv3, int texid);

void decode_3d_texcoord(union texcoord_3d texcoord, union vec2 *result);

#endif
