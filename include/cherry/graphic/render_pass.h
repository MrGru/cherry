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
#ifndef __CHERRY_GRAPHIC_RENDER_PASS_H__
#define __CHERRY_GRAPHIC_RENDER_PASS_H__

#include <cherry/graphic/types.h>

struct render_pass *render_pass_main_alloc();

struct render_pass *render_pass_depth_alloc();

void render_pass_begin(struct render_pass *p, u8 frame);

void render_pass_end();

void render_pass_free(struct render_pass *p);

#endif
