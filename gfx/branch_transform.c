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
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/math/math.h>
#include <cherry/memory.h>

struct branch_transform *branch_transform_alloc(u8 bid, struct branch_transform_queue *queue)
{
        struct branch_transform *p      = smalloc(sizeof(struct branch_transform));
        p->bid                          = bid;
        p->update                       = 1;
        p->position                     = vec3((float[3]){0, 0, 0});
        p->scale                        = vec3((float[3]){1, 1, 1});
        p->size                         = vec3((float[3]){1, 1, 1});
        p->quat                         = quat_identity;
        p->parent = NULL;
        p->update_queue = queue;
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->branch_list);
        INIT_LIST_HEAD(&p->branch_head);
        INIT_LIST_HEAD(&p->child_updater_list);
        INIT_LIST_HEAD(&p->updater_head);
        INIT_LIST_HEAD(&p->update_queue_head);
        return p;
}

void branch_transform_free(struct branch_transform *p)
{
        list_del(&p->tree_head);
        list_del(&p->branch_head);
        struct list_head *head;
        list_while_not_singular(head, &p->branch_list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, branch_head));
                branch_transform_free(b);
        }
        sfree(p);
}

void branch_transform_add(struct branch_transform *parent, struct branch_transform *child)
{
        if(!parent || !child) return;

        list_del(&child->branch_head);
        list_add_tail(&child->branch_head, &parent->branch_list);
        child->parent = parent;
}

void branch_transform_del(struct branch_transform *p)
{
        list_del(&p->branch_head);
        p->parent = NULL;
}

void branch_transform_shake(struct branch_transform *p)
{
        if(p->update) return;

        p->update = 1;
        u8 attached = 0;

        /*
         * remove all sub branchs from queue
         * p->update_queue->list will be small, I believe it
         */
        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->update_queue->list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, update_queue_head));
                if(b == p) {
                        INIT_LIST_HEAD(&p->child_updater_list);
                        list_del(&p->update_queue_head);
                        struct branch_transform *parent = p->parent;
                        if(parent) {
                                list_add_tail(&p->updater_head, &parent->child_updater_list);
                                list_add_tail(&parent->update_queue_head, &p->update_queue->list);
                        } else {
                                p->update_queue->full = 1;
                                p->update_queue->root = p;
                        }
                        attached = 1;
                } else {
                        struct branch_transform *parent = b->parent;
                        while(parent) {
                                if(parent->update) {
                                        INIT_LIST_HEAD(&b->child_updater_list);
                                        list_del(&b->update_queue_head);
                                        break;
                                }
                        }
                }
        }

        /*
         * try add branch to parent update list 
         */
        if(!attached) {
                struct branch_transform *parent = p->parent;
                if(parent) {
                        list_add_tail(&p->updater_head, &parent->child_updater_list);
                        list_add_tail(&parent->update_queue_head, &p->update_queue->list);
                } else {
                        p->update_queue->full = 1;
                        p->update_queue->root = p;
                }
        }
}

void branch_transform_traverse(struct branch_transform *p, union mat4 cm)
{
        if(p->update) {
                union mat4 transform = mat4_identity;
                transform = mat4_translate(transform, p->position);
                transform = mat4_mul(transform, mat4_from_quat(p->quat));
                transform = mat4_scale(transform, p->scale);
                transform = mat4_mul(cm, transform);

                p->last_transform = transform;

                transform = mat4_scale(transform, p->size);

                if(!list_singular(&p->tree_head)) {
                        struct list_head *head = p->tree_head.next;
                        struct list_head *node_head = (struct list_head *)
                                ((void *)head - p->offset_to_node);
                        if(!list_singular(node_head)) {
                                struct node *node = (struct node *)
                                        ((void *)node_head->next - offsetof(struct node, user_head));
                                node_set_data(node, p->bid, transform.m, sizeof(transform));
                        }
                }
        }

        struct list_head *head;
        list_for_each(head, &p->branch_list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, branch_head));
                b->update |= p->update;
                branch_transform_traverse(b, p->last_transform);
        }
        p->update = 0;
}
