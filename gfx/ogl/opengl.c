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
#include <cherry/graphic/graphic.h>

#if GFX == OGL

/*
 * most opengles device support 8 active textures
 *
 * application should check for max textures at runtime
 * and change this value
 */
int opengl_max_textures         = 8;

/*
 * most versions of opengl from 2.1 and opengles from 2.0
 * support depth texture
 *
 * application should check for depth texture extension at runtime
 * and change this value
 */
int depth_texture_enable        = 1;

#endif
