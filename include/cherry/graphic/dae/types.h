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
#ifndef __CHERRY_GRAPHIC_DAE_TYPES_H__
#define __CHERRY_GRAPHIC_DAE_TYPES_H__

#include <cherry/graphic/types.h>

struct dae_mesh {
        struct string   *name;
        struct array    *vertex_1;
        struct array    *vertex_2;
        struct array    *vertex_3;

        struct array    *normal_1;
        struct array    *normal_2;
        struct array    *normal_3;

        struct array    *texcoords;

        struct array    *colors;
};

#endif
