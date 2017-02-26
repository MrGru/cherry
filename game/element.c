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
#include <cherry/game/element.h>
#include <cherry/list.h>
#include <cherry/game/gem.h>

void game_element_init(struct game_element *p)
{
        INIT_LIST_HEAD(&p->path_head);
        INIT_LIST_HEAD(&p->life_head);
        INIT_LIST_HEAD(&p->update_pos_head);
}

void game_element_free(struct game_element *p)
{
        if(element_is_gem(p)) {
                struct gem *gem = (struct gem *)
                        ((void *)p - offsetof(struct gem, elm));
                gem_free(gem);
        }
}

void game_element_update_pos(struct game_element *p)
{
        if(element_is_gem(p)) {
                struct gem *gem = (struct gem *)
                        ((void *)p - offsetof(struct gem, elm));
                gem_update_node(gem);
        }
}
