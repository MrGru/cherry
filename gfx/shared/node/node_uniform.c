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
#include <cherry/graphic/node.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/string.h>
#include <cherry/bytes.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/stdio.h>

static void __require_transform_uniform(struct node *p)
{
        struct uniform_buffer *ub = transform_uniform_buffer_alloc();
        union mat4 trans = mat4_identity;
        uniform_buffer_set(ub, TRANSFORM_UNIFORM_TRANSFORM, &trans, sizeof(trans));
        array_push(p->current_uniform_buffers, &ub);
}

int node_setup_uniform(struct node *p)
{
        struct uniform_buffer **ub;
        array_for_each(ub, p->current_uniform_buffers) {
                uniform_buffer_free(*ub);
        }
        array_force_len(p->current_uniform_buffers, 0);

        switch (p->shader_type) {
                case SHADER_3D_TEXTURE_COLOR:
                        __require_transform_uniform(p);
                        break;
                case SHADER_2D_TEXTURE_COLOR:
                        __require_transform_uniform(p);
                        break;
                default:
                        debug("has not implemented setting up uniform for shader type %d\n", p->shader_type);
                        return 0;
        }

        return 1;
}
