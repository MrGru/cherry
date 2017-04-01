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
#include <cherry/graphic/node_manager.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/list.h>
#include <cherry/graphic/node.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>

struct node_manager *node_manager_alloc()
{
        struct node_manager *p          = smalloc(sizeof(struct node_manager));
        p->common_uniform_buffers       = map_alloc(sizeof(struct uniform_buffer *));
        p->shaders                      = map_alloc(sizeof(struct shader *));
        INIT_LIST_HEAD(&p->transform_queue);
        p->transform_root               = NULL;
        p->transform_full               = 0;
        INIT_LIST_HEAD(&p->nodes);
        return p;
}

void node_manager_free(struct node_manager *p)
{
        map_deep_free(p->common_uniform_buffers, struct uniform_buffer *, uniform_buffer_free);
        map_free(p->shaders);

        struct list_head *head;
        list_while_not_singular(head, &p->nodes) {
                struct node *n = (struct node *)
                        ((void *)head - offsetof(struct node, life_head));
                node_free(n);
        }
        sfree(p);
}

void node_manager_update_transform(struct node_manager *p)
{
        if(p->transform_full) {
                node_update_transform(p->transform_root);
                p->transform_root = NULL;
                p->transform_full = 0;
        } else {
                struct list_head *head, *next, *bhead, *bnext;
                list_for_each_safe(head, next, &p->transform_queue) {
                        struct node *parent = (struct node *)
                                ((void *)head - offsetof(struct node, transform_queue_head));
                        list_for_each_safe(bhead, bnext, &parent->updating_transform_children) {
                                struct node *child = (struct node *)
                                        ((void *)bhead - offsetof(struct node, updater_head));
                                node_update_transform(child);
                        }
                        INIT_LIST_HEAD(&parent->updating_transform_children);
                        list_del_init(head);
                }
        }
}
