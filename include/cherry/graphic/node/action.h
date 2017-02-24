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
#ifndef __CHERRY_GRAPHIC_NODE_ACTION_H__
#define __CHERRY_GRAPHIC_NODE_ACTION_H__

#include <cherry/graphic/node/types.h>

struct action *action_alloc(union vec4 *target, union vec4 offset, float duration, u8 type, i16 repeat);

struct action *action_alloc_gravity(union vec4 *target, float velocity, float accelerate, ...);

void action_free(struct action *p);

u8 action_update(struct action *p, float delta);

struct action *action_sequence(struct action *p, ...);

struct action *action_parallel(struct action *p, ...);

/*
 * action manager
 */
struct action_manager *action_manager_alloc();

void action_manager_update(struct action_manager *p, float delta);

void action_manager_free(struct action_manager *p);

void action_manager_add_key(struct action_manager *p, struct action_key * k);

void action_key_add_action(struct action_key *p, struct action * a);

void action_key_init(struct action_key *p);

#endif
