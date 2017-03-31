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
#ifndef __CHERRY_GRAPHIC_NODE_H__
#define __CHERRY_GRAPHIC_NODE_H__

#include <cherry/graphic/types.h>

struct node *node_alloc(struct node_manager *m);

void node_free(struct node *p);

void node_show_sprite(struct node *p, u32 shader_type, struct texture *tex);

void node_request_update_transform(struct node *p);

void node_update_transform(struct node *p);

#if GFX == OGL
void node_setup_buffer(struct device_buffer *buffer, struct shader *s, u32 key);
#endif

int node_setup_common_uniform(struct node *p);
int node_setup_uniform(struct node *p);

void node_render(struct node *p, u8 frame);

#endif
