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
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/graphic/render.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>

/*
 * detachs
 */
static inline void detach_node(struct node_3d_color *p)
{
        if(!list_singular(&p->node_head)) {
                struct list_head *head = p->node_head.next;
                struct node *node = (struct node *)
                        ((void *)head - offsetof(struct node, user_head));
                node_free(node);
        }
}

static inline void detach_transform(struct node_3d_color *p)
{
        if( ! list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                branch_transform_free(ob);
        }
}

static inline void detach_color(struct node_3d_color *p)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                branch_color_free(ob);
        }
}

static inline void detach_vertex(struct node_3d_color *p)
{
        if( ! list_singular(&p->vertex)) {
                struct list_head *head = p->vertex.next;
                struct twig_3d_vertex *ob = (struct twig_3d_vertex *)
                        ((void *)head - offsetof(struct twig_3d_vertex, tree_head));
                twig_3d_vertex_free(ob);
        }
}

static inline void detach_normal(struct node_3d_color *p)
{
        if( ! list_singular(&p->normal)) {
                struct list_head *head = p->normal.next;
                struct twig_3d_normal *ob = (struct twig_3d_normal *)
                        ((void *)head - offsetof(struct twig_3d_normal, tree_head));
                twig_3d_normal_free(ob);
        }
}

static inline void detach_vertex_color(struct node_3d_color *p)
{
        if( ! list_singular(&p->vertex_color)) {
                struct list_head *head = p->vertex_color.next;
                struct twig_vertex_color *ob = (struct twig_vertex_color *)
                        ((void *)head - offsetof(struct twig_vertex_color, tree_head));
                twig_vertex_color_free(ob);
        }
}

static inline void __node_3d_set_node(struct node_3d_color *p, struct node *n)
{
        /* detach previous node and push it back to pool */
        detach_node(p);
        /* detach current node_tree that node n is attaching to */
        list_del(&n->user_head);
        /* attach node n to node_tree p */
        list_add_tail(&n->user_head, &p->node_head);
}

struct node_3d_color *node_3d_color_alloc(struct node *n)
{
        struct node_3d_color *p = smalloc(sizeof(struct node_3d_color));
        INIT_LIST_HEAD(&p->node_head);
        INIT_LIST_HEAD(&p->transform);
        INIT_LIST_HEAD(&p->color);
        INIT_LIST_HEAD(&p->vertex);
        INIT_LIST_HEAD(&p->normal);
        INIT_LIST_HEAD(&p->vertex_color);
        INIT_LIST_HEAD(&p->life_head);
        __node_3d_set_node(p, n);
        return p;
}

void node_3d_color_free(struct node_3d_color *p)
{
        detach_node(p);
        detach_transform(p);
        detach_color(p);
        detach_vertex(p);
        detach_normal(p);
        detach_vertex_color(p);
        list_del(&p->life_head);
        sfree(p);
}

void node_3d_color_add_node_3d_color(struct node_3d_color *p, struct node_3d_color *q)
{
        branch_transform_add(node_3d_color_get_branch_transform(p),
                node_3d_color_get_branch_transform(q));
        branch_color_add(node_3d_color_get_branch_color(p),
                node_3d_color_get_branch_color(q));
}

/*
 * node_3d_color setup branchs and twigs
 */
void node_3d_color_set_branch_transform(struct node_3d_color *p, struct branch_transform *b)
{
        /* detach previous branch */
        detach_transform(p);
        /* format branch b */
        list_del(&b->tree_head);
        /* attach branch b */
        list_add_tail(&b->tree_head, &p->transform);
        b->offset_to_node = offsetof(struct node_3d_color, transform);
}

void node_3d_color_set_branch_color(struct node_3d_color *p, struct branch_color *b)
{
        detach_color(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->color);
        b->offset_to_node = offsetof(struct node_3d_color, color);
}

void node_3d_color_set_twig_3d_vertex(struct node_3d_color *p, struct twig_3d_vertex *b)
{
        detach_vertex(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->vertex);
        b->offset_to_node = offsetof(struct node_3d_color, vertex);
}

void node_3d_color_set_twig_3d_normal(struct node_3d_color *p, struct twig_3d_normal *b)
{
        detach_normal(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->normal);
        b->offset_to_node = offsetof(struct node_3d_color, normal);
}

void node_3d_color_set_twig_vertex_color(struct node_3d_color *p, struct twig_vertex_color *b)
{
        detach_vertex_color(p);
        list_del(&b->tree_head);
        list_add_tail(&b->tree_head, &p->vertex_color);
        b->offset_to_node = offsetof(struct node_3d_color, vertex_color);
}

struct branch_transform *node_3d_color_get_branch_transform(struct node_3d_color *p)
{
        if( ! list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return ob;
        }
        return NULL;
}

struct branch_color *node_3d_color_get_branch_color(struct node_3d_color *p)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                return ob;
        }
        return NULL;
}

struct node *node_3d_color_get_node(struct node_3d_color *p)
{
        if(!list_singular(&p->node_head)) {
                struct list_head *head = p->node_head.next;
                struct node *node = (struct node *)
                        ((void *)head - offsetof(struct node, user_head));
                return node;
        }
        return NULL;
}

/*
 * node_3d_color manipulate datas
 */
void node_3d_color_set_position(struct node_3d_color *p, union vec3 v)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                ob->position = v;
                branch_transform_shake(ob);
        }
}

void node_3d_color_set_scale(struct node_3d_color *p, union vec3 v)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                branch_transform_shake(ob);
                ob->scale = v;
        }
}

void node_3d_color_set_rotation(struct node_3d_color *p, union vec4 quat)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                branch_transform_shake(ob);
                ob->quat = quat;
        }
}

void node_3d_color_set_size(struct node_3d_color *p, union vec3 size)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                branch_transform_shake(ob);
                ob->size = size;
        }
}

void node_3d_color_fill_vertex(struct node_3d_color *p, void *v1, void *v2, void *v3, u32 len)
{
        if( ! list_singular(&p->vertex)) {
                struct list_head *head = p->vertex.next;
                struct twig_3d_vertex *ob = (struct twig_3d_vertex *)
                        ((void *)head - offsetof(struct twig_3d_vertex, tree_head));
                twig_3d_vertex_fill(ob, v1, v2, v3, len);
        }
}

void node_3d_color_set_vertex_1(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->vertex)) {
                struct list_head *head = p->vertex.next;
                struct twig_3d_vertex *ob = (struct twig_3d_vertex *)
                        ((void *)head - offsetof(struct twig_3d_vertex, tree_head));
                twig_3d_vertex_set_segment_v1(ob, seg, bytes, len);
        }
}

void node_3d_color_set_vertex_2(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->vertex)) {
                struct list_head *head = p->vertex.next;
                struct twig_3d_vertex *ob = (struct twig_3d_vertex *)
                        ((void *)head - offsetof(struct twig_3d_vertex, tree_head));
                twig_3d_vertex_set_segment_v2(ob, seg, bytes, len);
        }
}

void node_3d_color_set_vertex_3(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->vertex)) {
                struct list_head *head = p->vertex.next;
                struct twig_3d_vertex *ob = (struct twig_3d_vertex *)
                        ((void *)head - offsetof(struct twig_3d_vertex, tree_head));
                twig_3d_vertex_set_segment_v3(ob, seg, bytes, len);
        }
}

void node_3d_color_fill_normal(struct node_3d_color *p, void *v1, void *v2, void *v3, u32 len)
{
        if( ! list_singular(&p->normal)) {
                struct list_head *head = p->normal.next;
                struct twig_3d_normal *ob = (struct twig_3d_normal *)
                        ((void *)head - offsetof(struct twig_3d_normal, tree_head));
                twig_3d_normal_fill(ob, v1, v2, v3, len);
        }
}

void node_3d_color_set_normal_1(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->normal)) {
                struct list_head *head = p->normal.next;
                struct twig_3d_normal *ob = (struct twig_3d_normal *)
                        ((void *)head - offsetof(struct twig_3d_normal, tree_head));
                twig_3d_normal_set_segment_v1(ob, seg, bytes, len);
        }
}

void node_3d_color_set_normal_2(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->normal)) {
                struct list_head *head = p->normal.next;
                struct twig_3d_normal *ob = (struct twig_3d_normal *)
                        ((void *)head - offsetof(struct twig_3d_normal, tree_head));
                twig_3d_normal_set_segment_v2(ob, seg, bytes, len);
        }
}

void node_3d_color_set_normal_3(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->normal)) {
                struct list_head *head = p->normal.next;
                struct twig_3d_normal *ob = (struct twig_3d_normal *)
                        ((void *)head - offsetof(struct twig_3d_normal, tree_head));
                twig_3d_normal_set_segment_v3(ob, seg, bytes, len);
        }
}

void node_3d_color_fill_vertex_color(struct node_3d_color *p, void *v1, u32 len)
{
        if( ! list_singular(&p->vertex_color)) {
                struct list_head *head = p->vertex_color.next;
                struct twig_vertex_color *ob = (struct twig_vertex_color *)
                        ((void *)head - offsetof(struct twig_vertex_color, tree_head));
                twig_vertex_color_fill(ob, v1, len);
        }
}

void node_3d_color_set_vertex_color(struct node_3d_color *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if( ! list_singular(&p->vertex_color)) {
                struct list_head *head = p->vertex_color.next;
                struct twig_vertex_color *ob = (struct twig_vertex_color *)
                        ((void *)head - offsetof(struct twig_vertex_color, tree_head));
                twig_vertex_color_set_segment(ob, seg, bytes, len);
        }
}

void node_3d_color_set_color(struct node_3d_color *p, union vec4 color)
{
        if( ! list_singular(&p->color)) {
                struct list_head *head = p->color.next;
                struct branch_color *ob = (struct branch_color *)
                        ((void *)head - offsetof(struct branch_color, tree_head));
                ob->color = color;
        }
}

union vec3 *node_3d_color_get_position(struct node_3d_color *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->position;
        }
        return NULL;
}

union vec3 *node_3d_color_get_scale(struct node_3d_color *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->scale;
        }
        return NULL;
}

union vec4 *node_3d_color_get_rotation(struct node_3d_color *p)
{
        if(!list_singular(&p->transform)) {
                struct list_head *head = p->transform.next;
                struct branch_transform *ob = (struct branch_transform *)
                        ((void *)head - offsetof(struct branch_transform, tree_head));
                return &ob->quat;
        }
        return NULL;
}
