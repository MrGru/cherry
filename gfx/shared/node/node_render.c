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
#include <cherry/array.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/device_buffer.h>

struct node_render_buffer_group *node_render_buffer_group_alloc()
{
        struct node_render_buffer_group *p      = smalloc(sizeof(struct node_render_buffer_group));
        p->group                                = device_buffer_group_alloc();
        p->blend_mode                           = BLEND_NONE;
        p->vertice_count                        = 0;
        p->textures                             = array_alloc(sizeof(struct texture *), ORDERED);
        return p;
}

void node_render_buffer_group_free(struct node_render_buffer_group *p)
{
        device_buffer_group_free(p->group);
        array_deep_free(p->textures, struct texture *, texture_free);
        sfree(p);
}

void node_render_buffer_group_clear_group(struct node_render_buffer_group *p)
{
        device_buffer_group_clear(p->group);
}

void node_render_buffer_group_clear_texture(struct node_render_buffer_group *p)
{
        struct texture **tex;
        array_for_each(tex, p->textures) {
                texture_free(*tex);
        }
        array_force_len(p->textures, 0);
}

void node_render_buffer_group_add_texture(struct node_render_buffer_group *p, struct texture *tex)
{
        tex->ref++;
        array_push(p->textures, &tex);
}

struct node_render_content *node_render_content_alloc()
{
        struct node_render_content *p   = smalloc(sizeof(struct node_render_content));
        p->buffer_groups                = array_alloc(sizeof(struct node_render_buffer_group *), ORDERED);
        p->actives                      = 0;
        return p;
}

void node_render_content_free(struct node_render_content *p)
{
        array_deep_free(p->buffer_groups, struct node_render_buffer_group *, node_render_buffer_group_free);
        sfree(p);
}

void node_render_content_reserve(struct node_render_content *p, int count)
{
        while(p->buffer_groups->len < count) {
                struct node_render_buffer_group *group = node_render_buffer_group_alloc();
                array_push(p->buffer_groups, &group);
        }
}

void node_render_content_clear(struct node_render_content *p)
{
        struct node_render_buffer_group **group;
        array_for_each(group, p->buffer_groups) {
                node_render_buffer_group_clear_group(*group);
                node_render_buffer_group_clear_texture(*group);
        }
        p->actives = 0;
}
