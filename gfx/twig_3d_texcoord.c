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

struct twig_3d_texcoord *twig_3d_texcoord_alloc(u8 bid)
{
        struct twig_3d_texcoord *p      = smalloc(sizeof(struct twig_3d_texcoord));
        p->bid                          = bid;
        p->texcoord                     = (union vec4){0, 0, 0, 0.01};
        INIT_LIST_HEAD(&p->tree_head);
        return p;
}

void twig_3d_texcoord_free(struct twig_3d_texcoord *p)
{
        list_del(&p->tree_head);
        sfree(p);
}

void twig_3d_texcoord_fill(struct twig_3d_texcoord *p, void *tex, u32 len)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, tex, len);
                }
        }
}

void twig_3d_texcoord_set_segment(struct twig_3d_texcoord *p, struct node_data_segment *seg, void *bytes, u32 len)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data_segment(node, p->bid, seg, bytes, len);
                }
        }
}
