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
#include <cherry/graphic/node/twig.h>
#include <cherry/graphic/render.h>
#include <cherry/list.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>

struct twig_3d_normal *twig_3d_normal_alloc(u8 bid[3])
{
        struct twig_3d_normal *p        = smalloc(sizeof(struct twig_3d_normal));
        u8 i;
        for_i(i, 3) {
                p->bid[i]               = bid[i];
        }
        INIT_LIST_HEAD(&p->tree_head);
        return p;
}

void twig_3d_normal_free(struct twig_3d_normal *p)
{
        list_del(&p->tree_head);
        sfree(p);
}

void twig_3d_normal_fill(struct twig_3d_normal *p, void *v1, void *v2, void *v3, u32 len)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid[0], v1, len);
                        node_set_data(node, p->bid[1], v2, len);
                        node_set_data(node, p->bid[2], v3, len);
                }
        }
}

static inline void __twig_3d_normal_set_segment(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len, u8 index)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data_segment(node, p->bid[index], seg, bytes, len);
                }
        }
}

void twig_3d_normal_set_segment_v1(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        __twig_3d_normal_set_segment(p, seg, bytes, len, 0);
}

void twig_3d_normal_set_segment_v2(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        __twig_3d_normal_set_segment(p, seg, bytes, len, 1);
}

void twig_3d_normal_set_segment_v3(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        __twig_3d_normal_set_segment(p, seg, bytes, len, 2);
}
