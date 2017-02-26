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
#include <cherry/game/deliver.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/game/gem.h>
#include <cherry/game/path.h>

struct element_deliver *element_deliver_alloc_path_point(struct path_point *pp)
{
        struct element_deliver *p       = smalloc(sizeof(struct element_deliver));
        p->type                         = ELEMENT_DELIVER_PATH_POINT;
        p->ppoint                       = pp;
        INIT_LIST_HEAD(&p->trace_head);
        INIT_LIST_HEAD(&p->life_head);
        return p;
}

struct element_deliver *element_deliver_alloc_gate(struct path_point *pp)
{
        struct element_deliver *p       = smalloc(sizeof(struct element_deliver));
        p->type                         = ELEMENT_DELIVER_GATE;
        p->ppoint                       = pp;
        INIT_LIST_HEAD(&p->trace_head);
        INIT_LIST_HEAD(&p->life_head);
        return p;
}

struct element_deliver *element_deliver_alloc_gem_list(struct list_head *list, union vec4 pos)
{
        struct element_deliver *p       = smalloc(sizeof(struct element_deliver));
        p->type                         = ELEMENT_DELIVER_POOL;
        p->element_list                 = list;
        p->position_expanded            = pos;
        INIT_LIST_HEAD(&p->trace_head);
        INIT_LIST_HEAD(&p->life_head);
        return p;
}

void element_deliver_free(struct element_deliver *p)
{
        list_del(&p->life_head);
        sfree(p);
}
