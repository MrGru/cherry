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

#if GFX == OGL

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

void node_render(struct node *p, u8 frame)
{
        if(p->type == -1) return;

        shader_use(p->current_shader);
        /*
         * update uniform
         */
        int i, j;
        for(i = 0, j = 0; j < p->current_common_uniform_buffers->len; i++, j++) {
                struct link_uniform *link       = array_get(p->current_shader->link_uniforms, struct link_uniform *, i);
                struct uniform_buffer *ub       = array_get(p->current_common_uniform_buffers, struct uniform_buffer *, j);
                if(ub->last_pipeline != p->current_shader || ub->update == 1) {
                        ub->last_pipeline       = p->current_shader;
                        uniform_buffer_force_update(ub, link);
                        ub->update              = 0;
                }
        }
        for(j = 0; j < p->current_uniform_buffers->len; i++, j++) {
                struct link_uniform *link       = array_get(p->current_shader->link_uniforms, struct link_uniform *, i);
                struct uniform_buffer *ub       = array_get(p->current_uniform_buffers, struct uniform_buffer *, j);
                ub->last_pipeline = p->current_shader;
                uniform_buffer_force_update(ub, link);
                ub->update = 0;
        }
        /*
         * update textures
         */
        struct texture **tex;
        array_for_each_index(tex, i, p->textures) {
                texture_bind(*tex, i);
        }
        /*
         * render
         */
        device_buffer_group_bind_draw(p->current_buffer_group[frame]);
        glDrawArrays(GL_TRIANGLES, 0, p->vertice_count);
}

#endif
