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

struct twig_bright *twig_bright_alloc(u8 bid)
{
        struct twig_bright *p   = smalloc(sizeof(struct twig_bright));
        p->bid                  = bid;
        p->bright_expanded      = (union vec4){1, 0, 0, 0};
        INIT_LIST_HEAD(&p->tree_head);
        return p;
}

void twig_bright_free(struct twig_bright *p)
{
        list_del(&p->tree_head);
        sfree(p);
}

void twig_bright_update(struct twig_bright *p, float bright)
{
        p->bright = bright;
        if(!list_singular(&p->tree_head)) {
                struct list_head *head = p->tree_head.next;
                struct list_head *node_head = (struct list_head *)
                        ((void *)head - p->offset_to_node);
                if(!list_singular(node_head)) {
                        struct node *node = (struct node *)
                                ((void *)node_head->next - offsetof(struct node, user_head));
                        node_set_data(node, p->bid, &bright, sizeof(bright));
                }
        }
}