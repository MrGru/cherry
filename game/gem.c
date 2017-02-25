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
#include <cherry/game/gem.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/graphic/node/action.h>
#include <cherry/math/math.h>

struct gem *gem_alloc(u16 type)
{
        struct gem *p   = smalloc(sizeof(struct gem));
        INIT_LIST_HEAD(&p->elm.path_head);
        p->elm.type     = type;
        p->node         = NULL;
        p->flipped_node = NULL;

        action_key_init(&p->node_move_key);
        action_key_init(&p->node_choice_key);
        action_key_init(&p->node_unchoice_key);
        action_key_init(&p->node_collected_key);
        return p;
}

void gem_free(struct gem *p)
{
        action_key_clear(&p->node_move_key);
        action_key_clear(&p->node_choice_key);
        action_key_clear(&p->node_unchoice_key);
        action_key_clear(&p->node_collected_key);

        list_del(&p->elm.path_head);
        sfree(p);
}

void gem_set_node(struct gem *p, struct node_3d_color *node, struct node_3d_color *flipped_node)
{
        p->node = node;
        p->flipped_node = flipped_node;
}
