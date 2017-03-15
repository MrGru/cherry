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

/*
 * twig vertex 3d
 */
struct twig_3d_vertex *twig_3d_vertex_alloc(u8 bid[3]);

void twig_3d_vertex_free(struct twig_3d_vertex *p);

void twig_3d_vertex_fill(struct twig_3d_vertex *p, void *v1, void *v2, void *v3, u32 len);
void twig_3d_vertex_set_segment_v1(struct twig_3d_vertex *p, struct node_data_segment *seg, void *bytes, u32 len);
void twig_3d_vertex_set_segment_v2(struct twig_3d_vertex *p, struct node_data_segment *seg, void *bytes, u32 len);
void twig_3d_vertex_set_segment_v3(struct twig_3d_vertex *p, struct node_data_segment *seg, void *bytes, u32 len);

/*
 * twig normal 3d
 */
 struct twig_3d_normal *twig_3d_normal_alloc(u8 bid[3]);

 void twig_3d_normal_free(struct twig_3d_normal *p);

 void twig_3d_normal_fill(struct twig_3d_normal *p, void *n1, void *n2, void *n3, u32 len);
 void twig_3d_normal_set_segment_v1(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len);
 void twig_3d_normal_set_segment_v2(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len);
 void twig_3d_normal_set_segment_v3(struct twig_3d_normal *p, struct node_data_segment *seg, void *bytes, u32 len);

/*
 * twig texcoord 3d
 */
struct twig_3d_texcoord *twig_3d_texcoord_alloc(u8 bid);
void twig_3d_texcoord_free(struct twig_3d_texcoord *p);

void twig_3d_texcoord_fill(struct twig_3d_texcoord *p, void *tex, u32 len);
void twig_3d_texcoord_set_segment(struct twig_3d_texcoord *p, struct node_data_segment *seg, void *bytes, u32 len);

/*
 * twig vertex color
 */
 struct twig_vertex_color *twig_vertex_color_alloc(u8 bid);

 void twig_vertex_color_free(struct twig_vertex_color *p);

 void twig_vertex_color_fill(struct twig_vertex_color *p, void *v, u32 len);
 void twig_vertex_color_set_segment(struct twig_vertex_color *p, struct node_data_segment *seg, void *bytes, u32 len);

/*
 * twig vertex 2d
 */
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

/*
 * twig bright definitions
 */
struct twig_bright *twig_bright_alloc(u8 bid);

void twig_bright_free(struct twig_bright *p);

void twig_bright_update(struct twig_bright *p, float bright);

#endif
