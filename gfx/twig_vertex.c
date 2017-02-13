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

struct twig_vertex *twig_vertex_alloc(u8 bid[3])
{
        struct twig_vertex *p           = smalloc(sizeof(struct twig_vertex));
        u8 i;
        for_i(i, 3) {
                p->bid[i]               = bid[i];
                p->vertex[i]            = vec4((float[4]){0, 0, 0, 0});
        }
        INIT_LIST_HEAD(&p->tree_head);
        return p;
}

void twig_vertex_free(struct twig_vertex *p)
{
        list_del(&p->tree_head);
        sfree(p);
}

void twig_vertex_update(struct twig_vertex *p)
{
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        u8 i;
                        for_i(i, 3) {
                                node_set_data(node, p->bid[i], &p->vertex[i], sizeof(p->vertex[i]));
                        }
                }
        }
}
