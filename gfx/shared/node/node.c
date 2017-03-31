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

struct node *node_alloc(struct node_manager *m)
{
        struct node *p          = smalloc(sizeof(struct node));
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->transform_queue_head);
        INIT_LIST_HEAD(&p->updater_head);
        INIT_LIST_HEAD(&p->updating_transform_children);
        p->parent               = NULL;
        p->type                 = -1;
        p->shader_type          = -1;
        p->position_expand      = (union vec4){0, 0, 0, 0};
        p->scale_expand         = (union vec4){1, 1, 1, 0};
        p->size_expand          = (union vec4){1, 1, 1, 0};
        p->origin_expand        = (union vec4){0, 0, 0, 0};
        p->quaternion           = quat_identity;
        p->quaternion_animation = quat_identity;
        p->data                 = NULL;
        p->manager              = m;
        p->vertice_count        = 0;
        p->update               = 0;
        int i;
        for_i(i, BUFFERS) {
                p->current_buffer_group[i]      = NULL;
        }
        p->current_common_uniform_buffers       = array_alloc(sizeof(struct uniform_buffer *), ORDERED);
        p->current_uniform_buffers              = array_alloc(sizeof(struct uniform_buffer *), ORDERED);
        p->textures                             = array_alloc(sizeof(struct texture *), ORDERED);
        p->current_shader                       = NULL;
        return p;
}

void node_free(struct node *p)
{
        int i;
        for_i(i, BUFFERS) {
                device_buffer_group_free(p->current_buffer_group[i]);
        }
        array_free(p->current_common_uniform_buffers);
        array_deep_free(p->current_uniform_buffers, struct uniform_buffer *, uniform_buffer_free);
        array_deep_free(p->textures, struct texture *, texture_free);

        list_del(&p->head);
        list_del(&p->transform_queue_head);
        list_del(&p->updater_head);
        list_del(&p->life_head);

        if(p->manager->transform_root == p) {
                p->manager->transform_root = NULL;
                p->manager->transform_full = 0;
        }

        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->children) {
                struct node *n = (struct node *)
                        ((void *)head - offsetof(struct node, head));
                node_free(n);
        }
        sfree(p);
}

void node_request_update_transform(struct node *p)
{
        if(p->update) return;

        p->update       = 1;
        u8 attached     = 0;

        if(p->manager->transform_full) return;

        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->manager->transform_queue) {
                struct node *nq = (struct node *)
                        ((void *)head - offsetof(struct node, transform_queue_head));
                if(p == nq) {
                        INIT_LIST_HEAD(&p->updating_transform_children);
                        list_del_init(&p->transform_queue_head);
                        struct node *parent = p->parent;
                        if(parent) {
                                list_add_tail(&p->updater_head, &parent->updating_transform_children);
                                list_add_tail(&parent->transform_queue_head, &p->manager->transform_queue);
                        }
                        attached = 1;
                } else {
                        struct node *parent = nq->parent;
                        while(parent) {
                                if(parent->update) {
                                        INIT_LIST_HEAD(&nq->updating_transform_children);
                                        list_del_init(&nq->transform_queue_head);
                                        break;
                                }
                                parent = parent->parent;
                        }
                }
        }
        if(!attached) {
                struct node *parent = p->parent;
                if(parent) {
                        list_add_tail(&p->updater_head, &parent->updating_transform_children);
                        list_del(&parent->transform_queue_head);
                        list_add_tail(&parent->transform_queue_head, &p->manager->transform_queue);
                } else {
                        p->manager->transform_full = 1;
                        p->manager->transform_root = p;
                }
        }
}

void node_update_transform(struct node *p)
{
        
}
