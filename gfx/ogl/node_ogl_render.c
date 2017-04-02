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
#include <cherry/graphic/render_pass.h>

static struct render_pass *current_pass = NULL;

void render_pass_begin(struct render_pass *p, u8 frame)
{
        if(current_pass != p) {
                current_pass = p;
                glBindFramebuffer(GL_FRAMEBUFFER, current_pass->id);
        }
        if(current_pass->frame != frame) {
                current_pass->frame = frame;
                glClearColor(1, 1, 1, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
}

void render_pass_end()
{

}

static u8 current_blend = BLEND_NONE;

static void __blend(u8 mode)
{
        if(current_blend == mode) return;

        current_blend = mode;

        switch (current_blend) {
                case BLEND_NONE:
                        glDisable(GL_BLEND);
                        break;
                case BLEND_ALPHA:
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        break;
                case BLEND_MULTIPLY:
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
                        break;
                case BLEND_ADDITIVE:
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_ONE, GL_ONE);
                        break;
                default:
                        glEnable(GL_BLEND);
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        break;
        }
}

void node_render(struct node *p, u8 frame)
{
        if(!current_pass || !p->visible) return;

        if(p->type == -1 || p->shader_type == -1) goto render_children;

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
         * render
         */
        for_i(i, p->current_render_content[frame]->actives) {
                struct node_render_buffer_group *group = array_get(p->current_render_content[frame]->buffer_groups,
                        struct node_render_buffer_group *, i);
                /*
                 * update textures
                 */
                struct texture **tex;
                array_for_each_index(tex, j, group->textures) {
                        texture_bind(*tex, j);
                }
                /*
                 * update blend
                 */
                __blend(group->blend_mode);
                /*
                * draw
                 */
                device_buffer_group_bind_draw(group->group);
                glDrawArrays(GL_TRIANGLES, 0, group->vertice_count);
        }

render_children:;
        struct list_head *head;
        list_for_each(head, &p->children) {
                struct node *n = (struct node *)
                        ((void *)head - offsetof(struct node, head));
                node_render(n, frame);
        }
}

#endif
