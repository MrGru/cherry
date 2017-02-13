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
#include <cherry/graphic/shader.h>
#include <cherry/array.h>

/*
 * @p           : pipeline to manipulate
 * @index       : uniform track index
 * @u           : uniform set to index position
 */
void shader_set_uniform(struct shader *p, i16 index, struct shader_uniform *u)
{
        if(index < p->tracks->len) {
                shader_uniform_track_set(array_get(p->tracks, struct shader_uniform_track *, index), u);
        }
}
