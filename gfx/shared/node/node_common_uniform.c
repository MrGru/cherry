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
#include <cherry/stdio.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/device_buffer.h>

static int __setup_uniform(struct node *p, u32 key)
{
        struct uniform_buffer *ub       = map_get(p->manager->common_uniform_buffers, struct uniform_buffer *, &key, sizeof(key));
        if(!ub) {
                debug("lack of common uniform with key %d\n", key);
                return 0;
        }
        array_push(p->current_common_uniform_buffers, &ub);
        return 1;
}

int node_setup_common_uniform(struct node *p)
{
        array_force_len(p->current_common_uniform_buffers, 0);

#define SETUP_UNIFORM(key) \
        if(!__setup_uniform(p, key)) goto fail;

        switch (p->shader_type) {
                case SHADER_3D_TEXTURE_COLOR:
                        SETUP_UNIFORM(UNIFORM_CAMERA);
                        break;
                case SHADER_2D_TEXTURE_COLOR:
                        SETUP_UNIFORM(UNIFORM_CAMERA);
                        break;
                default:
                        break;
        }
#undef SETUP_UNIFORM

success:;
        return 1;
fail:;
        array_force_len(p->current_common_uniform_buffers, 0);
        return 0;
}
