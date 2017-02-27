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
#include <cherry/game/game.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/render.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>
#include <cherry/list.h>
#include <cherry/graphic/dae/dae_mesh.h>
#include <cherry/xml/xml.h>

static struct node_3d_color *__game_n3d_color_alloc(struct game *p, struct n3d_color_param *param)
{
        struct node_3d_color *n = node_3d_color_alloc(node_alloc(p->game_content));
        node_3d_color_set_branch_transform(n, branch_transform_alloc(1, p->n3d_update_queue));
        node_3d_color_set_branch_color(n, branch_color_alloc(2));
        u8 vbid[3] = {3, 4, 5};
        node_3d_color_set_twig_3d_vertex(n, twig_3d_vertex_alloc(vbid));
        u8 nbid[3] = {6, 7, 8};
        node_3d_color_set_twig_3d_normal(n, twig_3d_normal_alloc(nbid));
        node_3d_color_set_twig_vertex_color(n, twig_vertex_color_alloc(9));
        list_add_tail(&n->life_head, &p->node_3d_color_list);

        /*
         * we must fill all buffers datas that node reserved
         */
        /* fill vertex buffer */
        node_3d_color_fill_vertex(n, param->v1, param->v2, param->v3, param->vlen);
        /* fill normal buffer */
        node_3d_color_fill_normal(n, param->n1, param->n2, param->n3, param->vlen);
        /* fill vertex color buffer */
        node_3d_color_fill_vertex_color(n, param->color, param->vlen);
        /* fill transform buffer */
        node_3d_color_set_size(n, param->size);
        node_3d_color_set_position(n, vec3((float[3]){0, 0, 0}));
        /* fill color buffer */
        node_3d_color_set_color(n, (union vec4){1, 1, 1, 1});

        return n;
}

struct node_3d_color *game_gem_alloc(struct game *p, struct dae_mesh *mesh)
{
        struct n3d_color_param n3d_param;
        float size = 89;
        n3d_param.size = vec3((float[3]){size, size, size});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        int count = (int)(ceil((float)mesh->vertex_1->len / p->game_content->instance_multiple));

        array_reserve(mesh->vertex_1, p->game_content->instance_multiple * count);
        array_reserve(mesh->vertex_2, p->game_content->instance_multiple * count);
        array_reserve(mesh->vertex_3, p->game_content->instance_multiple * count);
        array_reserve(mesh->normal_1, p->game_content->instance_multiple * count);
        array_reserve(mesh->normal_2, p->game_content->instance_multiple * count);
        array_reserve(mesh->normal_3, p->game_content->instance_multiple * count);
#define APPEND_POINT(arr)                                               \
        while (arr->len < p->game_content->instance_multiple * count) { \
                union vec3 p = (union vec3){0, 0, 0};           \
                array_push(arr, &p);                            \
        }

        APPEND_POINT(mesh->vertex_1)
        APPEND_POINT(mesh->vertex_2)
        APPEND_POINT(mesh->vertex_3)
        APPEND_POINT(mesh->normal_1)
        APPEND_POINT(mesh->normal_2)
        APPEND_POINT(mesh->normal_3)
        APPEND_POINT(mesh->colors)
#undef APPEND_POINT

        n3d_param.vlen  = vsize;
        n3d_param.v1    = mesh->vertex_1->ptr;
        n3d_param.v2    = mesh->vertex_2->ptr;
        n3d_param.v3    = mesh->vertex_3->ptr;
        n3d_param.n1    = mesh->normal_1->ptr;
        n3d_param.n2    = mesh->normal_2->ptr;
        n3d_param.n3    = mesh->normal_3->ptr;
        n3d_param.color = mesh->colors->ptr;

        u32 i;
        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        node_3d_color_get_branch_color(n3d)->pass = 1;
        for_i(i, count - 1) {
                n3d_param.v1    = mesh->vertex_1->ptr + (i + 1) * vsize;
                n3d_param.v2    = mesh->vertex_2->ptr + (i + 1) * vsize;
                n3d_param.v3    = mesh->vertex_3->ptr + (i + 1) * vsize;
                n3d_param.n1    = mesh->normal_1->ptr + (i + 1) * vsize;
                n3d_param.n2    = mesh->normal_2->ptr + (i + 1) * vsize;
                n3d_param.n3    = mesh->normal_3->ptr + (i + 1) * vsize;
                n3d_param.color = mesh->colors->ptr   + (i + 1) * vsize;
                struct node_3d_color *sub = __game_n3d_color_alloc(p, &n3d_param);
                node_3d_color_add_node_3d_color(n3d, sub);
        }
        return n3d;
}

struct node_3d_color *game_empty_node_alloc(struct game *p)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){1, 1, 1});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        n3d_param.v1 = smalloc(vsize);
        n3d_param.v2 = smalloc(vsize);
        n3d_param.v3 = smalloc(vsize);
        n3d_param.n1 = smalloc(vsize);
        n3d_param.n2 = smalloc(vsize);
        n3d_param.n3 = smalloc(vsize);
        smemset(n3d_param.v1, 0, vsize);
        smemset(n3d_param.v2, 0, vsize);
        smemset(n3d_param.v3, 0, vsize);
        smemset(n3d_param.n1, 0, vsize);
        smemset(n3d_param.n2, 0, vsize);
        smemset(n3d_param.n3, 0, vsize);

        n3d_param.color = smalloc(vsize);
        u32 i;
        union vec3 blank = (union vec3){
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255)
        };
        for_i(i, p->game_content->instance_multiple) {
                n3d_param.color[i] = blank;
        }

        n3d_param.vlen = vsize;
        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        sfree(n3d_param.color);
        return n3d;
}

struct node_3d_color *game_floor_node_alloc(struct game *p)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){200 * 12, 200 * 15, 1});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        n3d_param.v1 = smalloc(vsize);
        n3d_param.v2 = smalloc(vsize);
        n3d_param.v3 = smalloc(vsize);
        n3d_param.n1 = smalloc(vsize);
        n3d_param.n2 = smalloc(vsize);
        n3d_param.n3 = smalloc(vsize);
        smemset(n3d_param.v1, 0, vsize);
        smemset(n3d_param.v2, 0, vsize);
        smemset(n3d_param.v3, 0, vsize);
        smemset(n3d_param.n1, 0, vsize);
        smemset(n3d_param.n2, 0, vsize);
        smemset(n3d_param.n3, 0, vsize);
        n3d_param.v1[0] = (union vec3){-0.5,  0.5, 0.0};
        n3d_param.v2[0] = (union vec3){-0.5, -0.5, 0.0};
        n3d_param.v3[0] = (union vec3){ 0.5, -0.5, 0.0};
        n3d_param.v1[1] = (union vec3){-0.5,  0.5, 0.0};
        n3d_param.v2[1] = (union vec3){ 0.5,  0.5, 0.0};
        n3d_param.v3[1] = (union vec3){ 0.5, -0.5, 0.0};
        n3d_param.vlen = vsize;

        n3d_param.color = smalloc(vsize);
        u32 i;
        union vec3 blank = (union vec3){
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255)
        };
        for_i(i, p->game_content->instance_multiple) {
                n3d_param.color[i] = blank;
        }

        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        sfree(n3d_param.color);
        return n3d;
}

struct node_3d_color *game_cell_alloc(struct game *p, struct dae_mesh *mesh, int row)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){1, 1, 1});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        n3d_param.v1 = smalloc(vsize);
        n3d_param.v2 = smalloc(vsize);
        n3d_param.v3 = smalloc(vsize);
        n3d_param.n1 = smalloc(vsize);
        n3d_param.n2 = smalloc(vsize);
        n3d_param.n3 = smalloc(vsize);
        smemset(n3d_param.v1, 0, vsize);
        smemset(n3d_param.v2, 0, vsize);
        smemset(n3d_param.v3, 0, vsize);
        smemset(n3d_param.n1, 0, vsize);
        smemset(n3d_param.n2, 0, vsize);
        smemset(n3d_param.n3, 0, vsize);
        int i, j;
        for_i(i, 12) {
                union vec3 pos = (union vec3){(i - 5) * 200, row * 200, 0};
                for_i(j, 8) {

                        n3d_param.v1[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_1, union vec3, j), 100));
                        n3d_param.v2[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_2, union vec3, j), 100));
                        n3d_param.v3[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_3, union vec3, j), 100));

                        n3d_param.n1[i * 8 + j]    = (union vec3) {0 ,0, 1};
                        n3d_param.n2[i * 8 + j]    = (union vec3) {0 ,0, 1};
                        n3d_param.n3[i * 8 + j]    = (union vec3) {0 ,0, 1};
                }
        }
        n3d_param.vlen = vsize;

        n3d_param.color = smalloc(vsize);
        union vec3 blank = (union vec3){
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255)
        };
        for_i(i, p->game_content->instance_multiple) {
                n3d_param.color[i] = blank;
        }

        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        sfree(n3d_param.color);
        return n3d;
}

struct node_3d_color *game_plane_alloc(struct game *p, struct dae_mesh *mesh, int row)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){1, 1, 1});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        n3d_param.v1 = smalloc(vsize);
        n3d_param.v2 = smalloc(vsize);
        n3d_param.v3 = smalloc(vsize);
        n3d_param.n1 = smalloc(vsize);
        n3d_param.n2 = smalloc(vsize);
        n3d_param.n3 = smalloc(vsize);
        smemset(n3d_param.v1, 0, vsize);
        smemset(n3d_param.v2, 0, vsize);
        smemset(n3d_param.v3, 0, vsize);
        smemset(n3d_param.n1, 0, vsize);
        smemset(n3d_param.n2, 0, vsize);
        smemset(n3d_param.n3, 0, vsize);
        int i, j;
        for_i(i, 12) {
                union vec3 pos = (union vec3){(i - 5) * 200, row * 200, 0};
                for_i(j, 8) {

                        n3d_param.v1[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_1, union vec3, j), 87));
                        n3d_param.v2[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_2, union vec3, j), 87));
                        n3d_param.v3[i * 8 + j]    = vec3_add(pos, vec3_mul_scalar(array_get(mesh->vertex_3, union vec3, j), 87));

                        n3d_param.n1[i * 8 + j]    = (union vec3) {0 ,0, 1};
                        n3d_param.n2[i * 8 + j]    = (union vec3) {0 ,0, 1};
                        n3d_param.n3[i * 8 + j]    = (union vec3) {0 ,0, 1};
                }
        }
        n3d_param.vlen = vsize;

        n3d_param.color = smalloc(vsize);
        union vec3 blank = (union vec3){
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255),
                pack_rgb_to_float(255, 255, 255)
        };
        for_i(i, p->game_content->instance_multiple) {
                n3d_param.color[i] = blank;
        }

        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        sfree(n3d_param.color);
        return n3d;
}
