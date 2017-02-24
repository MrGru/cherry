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
#include <cherry/game/path.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/math/math.h>

struct path_point *path_point_alloc()
{
        struct path_point *p    = smalloc(sizeof(struct path_point));
        u8 i;
        for_i(i, 8) {
                INIT_LIST_HEAD(&p->neighbor[i]);
                INIT_LIST_HEAD(&p->neighbor_head[i]);
        }
        INIT_LIST_HEAD(&p->dynamic_element);
        INIT_LIST_HEAD(&p->obstacle_lv1);
        INIT_LIST_HEAD(&p->obstacle_lv2);
        INIT_LIST_HEAD(&p->request_dyelm_head);
        p->position_expaned     = (union vec4){0 , 0, 0, 0};
        return p;
}

void path_point_free(struct path_point *p)
{
        list_del(&p->request_dyelm_head);
        sfree(p);
}

void path_point_add_neighbor(struct path_point *p, struct path_point *n, u8 index)
{
        if(!list_singular(&p->neighbor[index])) {
                list_del_init(p->neighbor[index].next);
        }
        if(n) {
                u8 n_index = (index + 4) % 8;
                if(!list_singular(&n->neighbor_head[n_index])) {
                        list_del(&n->neighbor_head[n_index]);
                }
                list_add_tail(&n->neighbor_head[n_index], &p->neighbor[index]);
        }
}

void path_request_dynamic_element(struct path_point *p, struct game *context)
{
        
}
