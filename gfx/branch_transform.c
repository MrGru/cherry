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

struct rotation_vector *rotation_vector_alloc()
{
        struct rotation_vector *p = smalloc(sizeof(struct rotation_vector));
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->action);
        return p;
}

void rotation_vector_free(struct rotation_vector *p)
{
        list_del(&p->head);
        if(!list_singular(&p->action)) {
                list_del_init(p->action.next);
        }
        sfree(p);
}

struct branch_transform_queue *branch_transform_queue_alloc()
{
        struct branch_transform_queue *p = smalloc(sizeof(struct branch_transform_queue));
        INIT_LIST_HEAD(&p->list);
        p->root = NULL;
        p->full = 0;
        return p;
}

void branch_transform_queue_traverse(struct branch_transform_queue *p)
{
        if(p->full) {
                union mat4 m = mat4_identity;
                branch_transform_traverse(p->root, m);
                p->root = NULL;
                p->full = 0;
        } else {
                struct list_head *head, *next;
                list_for_each_safe(head, next, &p->list) {
                        struct branch_transform *b = (struct branch_transform *)
                                ((void *)head - offsetof(struct branch_transform, update_queue_head));
                        struct list_head *bhead, *bnext;
                        list_for_each_safe(bhead, bnext, &b->child_updater_list) {
                                struct branch_transform *bc = (struct branch_transform *)
                                        ((void *)bhead - offsetof(struct branch_transform, updater_head));
                                branch_transform_traverse(bc, b->last_transform);
                        }
                        INIT_LIST_HEAD(&b->child_updater_list);
                        list_del_init(head);
                }
        }
}

void branch_transform_queue_free(struct branch_transform_queue *p)
{
        branch_transform_queue_traverse(p);
        sfree(p);
}

struct branch_transform *branch_transform_alloc(u8 bid, struct branch_transform_queue *queue)
{
        struct branch_transform *p      = smalloc(sizeof(struct branch_transform));
        p->bid                          = bid;
        p->update                       = 0;
        p->position_expaned             = vec4((float[4]){0, 0, 0, 0});
        p->scale_expaned                = vec4((float[4]){1, 1, 1, 0});
        p->size_expaned                 = vec4((float[4]){1, 1, 1, 0});
        p->quat                         = quat_identity;
        p->parent = NULL;
        p->update_queue = queue;
        INIT_LIST_HEAD(&p->tree_head);
        INIT_LIST_HEAD(&p->branch_list);
        INIT_LIST_HEAD(&p->branch_head);
        INIT_LIST_HEAD(&p->child_updater_list);
        INIT_LIST_HEAD(&p->updater_head);
        INIT_LIST_HEAD(&p->update_queue_head);
        INIT_LIST_HEAD(&p->anim_rotations);
        return p;
}

void branch_transform_free(struct branch_transform *p)
{
        list_del(&p->tree_head);
        list_del(&p->branch_head);
        if(p->parent) {
                if(!list_singular(&p->parent->update_queue_head) && p->update) {
                        list_del(&p->updater_head);
                }
        }
        struct list_head *head;
        list_while_not_singular(head, &p->branch_list) {
                struct branch_transform *b = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, branch_head));
                branch_transform_free(b);
        }
        list_while_not_singular(head, &p->anim_rotations) {
                struct rotation_vector *rv = (struct rotation_vector *)
                        ((void *)head - offsetof(struct rotation_vector, head));
                rotation_vector_free(rv);
        }
        list_del(&p->update_queue_head);
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
        list_del_init(&p->branch_head);
        p->parent = NULL;
}

void branch_transform_shake(struct branch_transform *p)
{
        if(p->update) return;

        p->update = 1;
        u8 attached = 0;

        if(p->update_queue->full) return;

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
                        list_del_init(&p->update_queue_head);
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
                                        list_del_init(&b->update_queue_head);
                                        break;
                                }
                                parent = parent->parent;
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
                        list_del(&parent->update_queue_head);
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
                union vec4 aquat = quat_identity;
                struct list_head *rhead, *rnext;
                i16 i = 0;
                list_for_each_safe(rhead, rnext, &p->anim_rotations) {
                        struct rotation_vector *rv = (struct rotation_vector *)
                                ((void *)rhead - offsetof(struct rotation_vector, head));
                        if(i == 0 && list_singular(&rv->action)) {
                                p->quat = quat_mul(p->quat, quat_angle_axis(rv->rad_vec3.x, &rv->rad_vec3.y));
                                rotation_vector_free(rv);
                                i--;
                        } else {
                                aquat = quat_mul(aquat, quat_angle_axis(rv->rad_vec3.x, &rv->rad_vec3.y));
                        }
                        i++;
                }

                union mat4 transform = mat4_identity;
                transform = mat4_translate(transform, p->position);
                transform = mat4_mul(transform, mat4_from_quat(p->quat));
                if(i) transform = mat4_mul(transform, mat4_from_quat(aquat));
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
