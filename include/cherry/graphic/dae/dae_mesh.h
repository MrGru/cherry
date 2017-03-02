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
#ifndef __CHERRY_GRAPHIC_DAE_DAE_MESH_H__
#define __CHERRY_GRAPHIC_DAE_DAE_MESH_H__

#include <cherry/graphic/dae/types.h>

struct array *dae_mesh_alloc(char *file);

void dae_mesh_free(struct dae_mesh *p);

#endif
