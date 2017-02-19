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
#ifndef __CHERRY_BYTES_H__
#define __CHERRY_BYTES_H__

#include <cherry/types.h>

/*
 * allocate new bytes object
 */
struct bytes *bytes_alloc();

/*
 * deallocate bytes object p and content
 */
void bytes_free(struct bytes *p);

/*
 * resize bytes content big enough to hold size bytes
 */
void bytes_reserve(struct bytes *p, u32 size);

/*
 * append size bytes to p
 */
void bytes_cat(struct bytes *p, void *s, u32 size);

void bytes_sub(struct bytes *p, u32 offset, void *s, u32 size);

#endif
