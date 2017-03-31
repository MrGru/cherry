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
#include <cherry/graphic/link_uniform.h>
#include <cherry/memory.h>
#include <cherry/array.h>

struct link_uniform *link_uniform_alloc()
{
        struct link_uniform *p  = smalloc(sizeof(struct link_uniform));
        p->ids                  = array_alloc(sizeof(i32), ORDERED);
        return p;
}

void link_uniform_free(struct link_uniform *p)
{
        array_free(p->ids);
        sfree(p);
}
