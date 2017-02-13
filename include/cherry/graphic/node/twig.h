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
#ifndef __CHERRY_GRAPHIC_NODE_TWIG_H__
#define __CHERRY_GRAPHIC_NODE_TWIG_H__

#include <cherry/graphic/node/types.h>

struct twig_vertex *twig_vertex_alloc(u8 bid[3]);

void twig_vertex_free(struct twig_vertex *p);

void twig_vertex_update(struct twig_vertex *p);

/*
 * twig texcoord definitions
 */
struct twig_texcoord *twig_texcoord_alloc(u8 bid[3]);

void twig_texcoord_free(struct twig_texcoord *p);

void twig_texcoord_update(struct twig_texcoord *p);

/*
 * twig texid definitions
 */
struct twig_texid *twig_texid_alloc(u8 bid);

void twig_texid_free(struct twig_texid *p);

void twig_texid_update(struct twig_texid *p);

#endif
