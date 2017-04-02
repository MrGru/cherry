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

/*
 * sprite 2d input
 */
static float sprite_2d_position_input[12] = {
        -0.5,  0.5,
        -0.5, -0.5,
         0.5, -0.5,

        -0.5,  0.5,
         0.5,  0.5,
         0.5, -0.5
};

static float sprite_2d_texcoord_input[12] = {
        0, 0,
        0, 1,
        1, 1,

        0, 0,
        1, 0,
        1, 1
};

static float sprite_2d_color_input[24] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,

        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
};

static inline void __setup_buffer(struct node *p, struct device_buffer_group *group, u16 item_size, u8 location, void *data, size_t len, u32 key)
{
        struct device_buffer *db = device_buffer_alloc(BUFFER_VERTICE, item_size, location);
        device_buffer_fill(db, data, len);
        device_buffer_group_add(group, db);
#if GFX == OGL
        node_setup_buffer(db, p->current_shader, key);
#endif
}

static void __add_texture_shader_2d_texture_color(struct node *p, struct texture *tex)
{
        int i;
        for_i(i, BUFFERS) {
                if(p->current_render_content[i]->buffer_groups->len == 1) {
                        struct node_render_buffer_group *group  = array_get(p->current_render_content[i]->buffer_groups,
                                struct node_render_buffer_group *, 0);
                        node_render_buffer_group_clear_texture(group);
                        node_render_buffer_group_add_texture(group, tex);
                }
        }
}

static void __setup_shader_2d_texture_color(struct node *p, struct texture *tex)
{
        int i;
        for_i(i, BUFFERS) {
                node_render_content_reserve(p->current_render_content[i], 1);
                p->current_render_content[i]->actives   = 1;
                struct node_render_buffer_group *group  = array_get(p->current_render_content[i]->buffer_groups,
                        struct node_render_buffer_group *, 0);
                group->vertice_count                    = 6;
                group->blend_mode                       = BLEND_ALPHA;
                node_render_buffer_group_add_texture(group, tex);
                device_buffer_group_bind_construct(group->group);

                __setup_buffer(p, group->group, sizeof(union vec2), BUFFER_DEVICE,
                        sprite_2d_position_input, sizeof(sprite_2d_position_input), INPUT_POSITION_2D);
                __setup_buffer(p, group->group, sizeof(union vec2), BUFFER_DEVICE,
                        sprite_2d_texcoord_input, sizeof(sprite_2d_texcoord_input), INPUT_TEXCOORD);
                __setup_buffer(p, group->group, sizeof(union vec4), BUFFER_DEVICE,
                        sprite_2d_color_input, sizeof(sprite_2d_color_input), INPUT_COLOR);

                device_buffer_group_bind_construct(NULL);
        }
}

void node_show_sprite(struct node *p, u32 shader_type, struct texture *tex)
{
        int i;
        switch (p->type) {
                case NODE_SPRITE:
                        if(p->shader_type == shader_type) goto assign_texture;
                        goto setup_input;
                default:
                        goto setup_input;
        }
fail:;
        p->type                 = -1;
        p->shader_type          = -1;
        p->current_shader       = NULL;
        for_i(i, BUFFERS) {
                if(p->current_render_content[i]) {
                        node_render_content_clear(p->current_render_content[i]);
                }
        }
        array_force_len(p->current_common_uniform_buffers, 0);
        array_force_len(p->current_uniform_buffers, 0);
        return;

setup_input:;
        p->type                 = NODE_SPRITE;
        p->shader_type          = shader_type;
        p->current_shader       = map_get(p->manager->shaders, struct shader *, &shader_type, sizeof(shader_type));
        if(!p->current_shader) goto fail;

        for_i(i, BUFFERS) {
                if(p->current_render_content[i]) {
                        node_render_content_clear(p->current_render_content[i]);
                } else {
                        p->current_render_content[i] = node_render_content_alloc();
                }
        }
        switch (shader_type) {
                case SHADER_2D_TEXTURE_COLOR:
                        __setup_shader_2d_texture_color(p, tex);
                        break;
                default:
                        goto fail;
        }

setup_uniform:;
        if(!node_setup_common_uniform(p)) goto fail;
        if(!node_setup_uniform(p)) goto fail;
        return;

assign_texture:;
        __add_texture_shader_2d_texture_color(p, tex);
}
