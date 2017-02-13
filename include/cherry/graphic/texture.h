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
#ifndef __CHERRY_GRAPHIC_TEXTURE_H__
#define __CHERRY_GRAPHIC_TEXTURE_H__

#include <cherry/graphic/types.h>

/*
 * get texture from cache by file
 */
struct texture *texture_alloc_file(char *file);

/*
 * allocate new texture from image
 */
struct texture *texture_alloc_image(struct image *p);

/*
 * allocate new depth texture
 */
struct texture *texture_alloc_depth(u16 width, u16 height);

/*
 * decrease texture's ref by 1
 * if the texture's ref equal to zero then deallocate it
 */
void texture_free(struct texture *p);

#if GFX == OGL
void texture_bind(struct texture *p);
#endif

/*
 * remove unused textures in cache
 */
void texture_remove_unused();

#endif
