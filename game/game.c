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
#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/graphic/shader/descriptor_collection.h>
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/shader.h>
#include <cherry/math/math.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/node/node_tree.h>
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>
#include <cherry/list.h>
#include <cherry/graphic/camera.h>
#include <cherry/graphic/dae/dae_mesh.h>
#include <cherry/graphic/light/light.h>

#define GAME_TRIANGLES_PER_OBJECT 100

/*
 * create ui content
 */
static inline void __game_create_ui_content(struct game *p, struct render_queue *queue)
{
        struct array *buffers[BUFFERS];
        u32 buffer_size = 900;
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_quad_alloc(BUFFER_DEVICE)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_z_alloc(buffer_size, BUFFER_DEVICE)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_transform_alloc(buffer_size, BUFFER_PINNED)});
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_color_alloc(buffer_size, BUFFER_DEVICE)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_texid_alloc(buffer_size, BUFFER_DEVICE)
                });
                u8 j;
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_texcoord_alloc(buffer_size, BUFFER_DEVICE)
                        });
                }
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_vertex_alloc(buffer_size, BUFFER_DEVICE)
                        });
                }
        }

        struct render_content *content = render_content_alloc(queue, buffers, 6, buffer_size, 1);
        content->depth_test = 1;
        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        p->ui_content = content;
}

struct node_tree *__game_ui_sprite_alloc(struct game *p, struct ui_sprite_param *param)
{
        struct node_tree *n = node_tree_alloc(node_alloc(p->ui_content));
        node_tree_set_branch_z(n, branch_z_alloc(1));
        node_tree_set_branch_transform(n, branch_transform_alloc(2, p->update_queue));
        node_tree_set_branch_color(n, branch_color_alloc(3));
        node_tree_set_twig_texid(n, twig_texid_alloc(4));
        u8 cbid[3] = {5, 6, 7};
        node_tree_set_twig_texcoord(n, twig_texcoord_alloc(cbid));
        u8 vbid[3] = {8, 9, 10};
        node_tree_set_twig_vertex(n, twig_vertex_alloc(vbid));
        list_add_tail(&n->life_head, &p->node_tree_list);

        u8 i;
        for_i(i, 6) {
                node_tree_set_texcoord(n, i, param->texcoord[i], i == 5);
        }

        node_tree_set_vertex(n, 0, vec2((float[2]){-0.5,  0.5}), 0);
        node_tree_set_vertex(n, 1, vec2((float[2]){-0.5, -0.5}), 0);
        node_tree_set_vertex(n, 2, vec2((float[2]){ 0.5, -0.5}), 0);
        node_tree_set_vertex(n, 3, vec2((float[2]){-0.5,  0.5}), 0);
        node_tree_set_vertex(n, 4, vec2((float[2]){ 0.5,  0.5}), 0);
        node_tree_set_vertex(n, 5, vec2((float[2]){ 0.5, -0.5}), 1);

        node_tree_set_texid(n, param->texid);
        node_tree_set_size(n, param->size);
        node_tree_set_position(n, vec3((float[3]){0, 0, 0}));

        return n;
}

/*
 * create game content
 */
static inline void __game_create_game_content(struct game *p, struct render_queue *queue)
{
        struct array *buffers[BUFFERS];
        u32 triangles_per_object = GAME_TRIANGLES_PER_OBJECT;
        u32 instances = 300;
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_triangle_alloc(BUFFER_DEVICE)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_transform_alloc(instances, BUFFER_PINNED)});
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_color_alloc(instances, BUFFER_DEVICE)
                });
                u8 j;
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_3d_vertex_alloc(instances * triangles_per_object, BUFFER_PINNED)
                        });
                }
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_3d_normal_alloc(instances * triangles_per_object, BUFFER_PINNED)
                        });
                }
        }

        struct render_content *content = render_content_alloc(queue, buffers, 3, instances, triangles_per_object);
        content->depth_test = 1;
        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        p->game_content = content;
}

struct node_3d_color *__game_n3d_color_alloc(struct game *p, struct n3d_color_param *param)
{
        struct node_3d_color *n = node_3d_color_alloc(node_alloc(p->game_content));
        node_3d_color_set_branch_transform(n, branch_transform_alloc(1, p->n3d_update_queue));
        node_3d_color_set_branch_color(n, branch_color_alloc(2));
        u8 vbid[3] = {3, 4, 5};
        node_3d_color_set_twig_3d_vertex(n, twig_3d_vertex_alloc(vbid));
        u8 nbid[3] = {6, 7, 8};
        node_3d_color_set_twig_3d_normal(n, twig_3d_normal_alloc(nbid));
        list_add_tail(&n->life_head, &p->node_3d_color_list);

        node_3d_color_fill_vertex(n, param->v1, param->v2, param->v3, param->vlen);
        node_3d_color_fill_normal(n, param->n1, param->n2, param->n3, param->vlen);

        node_3d_color_set_size(n, param->size);
        node_3d_color_set_position(n, vec3((float[3]){0, 0, 0}));

        return n;
}

struct node_3d_color *__game_gem_alloc(struct game *p, struct dae_mesh *mesh)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){100, 100, 100});
        u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
        array_reserve(mesh->vertex_1, p->game_content->instance_multiple);
        array_reserve(mesh->vertex_2, p->game_content->instance_multiple);
        array_reserve(mesh->vertex_3, p->game_content->instance_multiple);
        array_reserve(mesh->normal_1, p->game_content->instance_multiple);
        array_reserve(mesh->normal_2, p->game_content->instance_multiple);
        array_reserve(mesh->normal_3, p->game_content->instance_multiple);
#define APPEND_POINT(arr)                                               \
        while (arr->len < p->game_content->instance_multiple) { \
                union vec3 p = (union vec3){0, 0, 0};           \
                array_push(arr, &p);                            \
        }

        APPEND_POINT(mesh->vertex_1);
        APPEND_POINT(mesh->vertex_2);
        APPEND_POINT(mesh->vertex_3);
        APPEND_POINT(mesh->normal_1);
        APPEND_POINT(mesh->normal_2);
        APPEND_POINT(mesh->normal_3);
#undef APPEND_POINT

        n3d_param.vlen  = vsize;
        n3d_param.v1    = mesh->vertex_1->ptr;
        n3d_param.v2    = mesh->vertex_2->ptr;
        n3d_param.v3    = mesh->vertex_3->ptr;
        n3d_param.n1    = mesh->normal_1->ptr;
        n3d_param.n2    = mesh->normal_2->ptr;
        n3d_param.n3    = mesh->normal_3->ptr;
        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);

        return n3d;
}

struct node_3d_color *__game_empty_node_alloc(struct game *p)
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
        n3d_param.vlen = vsize;
        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        return n3d;
}

struct node_3d_color *__game_floor_node_alloc(struct game *p)
{
        struct n3d_color_param n3d_param;
        n3d_param.size = vec3((float[3]){4000, 6000, 1});
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
        struct node_3d_color *n3d = __game_n3d_color_alloc(p, &n3d_param);
        sfree(n3d_param.v1);
        sfree(n3d_param.v2);
        sfree(n3d_param.v3);
        sfree(n3d_param.n1);
        sfree(n3d_param.n2);
        sfree(n3d_param.n3);
        return n3d;
}

struct game *game_alloc()
{
        int i, j;
        struct game *p  = smalloc(sizeof(struct game));
        INIT_LIST_HEAD(&p->renderer_list);
        INIT_LIST_HEAD(&p->node_tree_list);
        INIT_LIST_HEAD(&p->node_3d_color_list);
        p->update_queue         = branch_transform_queue_alloc();
        p->n3d_update_queue     = branch_transform_queue_alloc();
        p->frame                = 0;

        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        renderer_set_color(r, &(union vec4){0, 0, 0, 1});
        list_add_tail(&r->chain_head, &p->renderer_list);

        /*
         * add game content
         */
        struct render_stage *stage = render_stage_alloc(r);

        struct shader *s3d_color = shader_3d_color_alloc(0, 1, 0);
        s3d_color->descriptor = descriptor_3d_color_get(GAME_TRIANGLES_PER_OBJECT);
        struct render_queue *s3d_queue = render_queue_alloc(&stage->content_queue_list, s3d_color);
        __game_create_game_content(p, s3d_queue);
        p->game_projection_uniform = shader_uniform_alloc();
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_PROJECT, p->game_projection_uniform);

        struct point_light *pl = point_light_alloc();
	point_light_set_position(pl, vec3((float[3]){0, 200, 1000}));
	point_light_set_constant(pl, 1.0f);
	point_light_set_linear(pl, 0.00014f);
	point_light_set_quadratic(pl, 0.00000001f);
	point_light_set_ambient(pl, vec3((float[3]){0.5, 0.5, 0.5}));
	point_light_set_diffuse(pl, vec3((float[3]){0.5, 0.5, 0.5}));
	point_light_set_specular(pl, vec3((float[3]){0.2, 0.2, 0.2}));

        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_POSITION, pl->u_position);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_CONSTANT, pl->u_constant);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_LINEAR, pl->u_linear);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_QUADRATIC, pl->u_quadratic);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_AMBIENT, pl->u_ambient);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_DIFFUSE, pl->u_diffuse);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_SPECULAR, pl->u_specular);

        p->world_light = pl;

        p->game_cam = camera_alloc(mat4_new_look_at(
                0, 0, 4100,
                0, 0, 0,
                0, 1, 0
        ));
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_VIEW, p->game_cam->view_uniform);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_VIEW_POSITION, p->game_cam->position_uniform);
        union mat4 project = mat4_new_perspective(DEG_TO_RAD(45.0f), video_width * 1.0f / video_height, 1.0f, 10000.0f);
        shader_uniform_update(p->game_projection_uniform, project.m, sizeof(project));

        camera_rotate_around(p->game_cam, quat_angle_axis(DEG_TO_RAD(-35), (float[3]){1, 0, 0}));

        int mesh_types = 1;
        struct dae_mesh *mesh[1] = {
                dae_mesh_alloc("res/models/gem_1.dae")
        };
        struct node_3d_color *n1 = __game_empty_node_alloc(p);
        union vec4 color[6] = {
                (union vec4){255 / 255.0f, 67 / 255.0f, 120 / 255.0f, 1},
                (union vec4){255 / 255.0f, 67 / 255.0f, 255 / 255.0f, 1},
                (union vec4){172 / 255.0f, 67 / 255.0f, 255 / 255.0f, 1},
                (union vec4){67 / 255.0f, 255 / 255.0f, 156 / 255.0f, 1},
                (union vec4){234 / 255.0f, 255 / 255.0f, 67 / 255.0f, 1},
                (union vec4){255 / 255.0f, 141 / 255.0f, 67 / 255.0f, 1},
        };
        for_i(i, 9) {
                for_i(j, 9) {
                        int mesh_type   = rand_ri(0, mesh_types);
                        int ct          = rand_ri(0, 6);
                        {
                                struct node_3d_color *n2 = __game_gem_alloc(p, mesh[mesh_type]);
                                node_3d_color_add_node_3d_color(n1, n2);
                                node_3d_color_set_position(n2, (union vec3){(i - 4) * 200, (j - 4) * 200, 0});
                                node_3d_color_set_color(n2, color[ct]);
                        }
                        {
                                struct node_3d_color *n2 = __game_gem_alloc(p, mesh[mesh_type]);
                                node_3d_color_add_node_3d_color(n1, n2);
                                node_3d_color_set_position(n2, (union vec3){(i - 4) * 200, (j - 4) * 200, -200});
                                node_3d_color_set_color(n2, vec4_mul_scalar(color[ct], 0.8));
                        }
                }
        }
        for_i(i, mesh_types) {
                dae_mesh_free(mesh[i]);
        }
        {
                struct node_3d_color *n2 = __game_floor_node_alloc(p);
                node_3d_color_add_node_3d_color(n1, n2);
                node_3d_color_set_position(n2, (union vec3){0, 0, -100});
                node_3d_color_set_color(n2, (union vec4){0.0, 0.0, 0.0, 0.7});
        }

        /* recalculate color tree */
        union vec4 v = vec4((float[4]){1, 1, 1, 1});
        branch_color_traverse(node_3d_color_get_branch_color(n1), v);


        /*
         * add ui content
         */
        struct shader *s = shader_texture_alloc();
        struct render_queue *queue = render_queue_alloc(&stage->content_queue_list, s);
        __game_create_ui_content(p, queue);

        p->ui_projection_uniform  = shader_uniform_alloc();
        shader_set_uniform(s, SHADER_TEXTURE_PROJECT, p->ui_projection_uniform);

        project = mat4_new_ortho(-video_width/2,
                video_width/2, -video_height/2, video_height/2, 1, 10);
        p->ui_cam = camera_alloc(mat4_new_look_at(
                0, 0, 1,
                0, 0, 0,
                0, 1, 0
        ));
        shader_set_uniform(s, SHADER_TEXTURE_VIEW, p->ui_cam->view_uniform);
        shader_uniform_update(p->ui_projection_uniform, project.m, sizeof(project));

        // /*
        //  * add node
        //  */
        // struct ui_sprite_param ui_param;
        // ui_param.texcoord[0]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[1]    = vec2((float[2]){0, 1});
        // ui_param.texcoord[2]    = vec2((float[2]){1, 1});
        // ui_param.texcoord[3]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[4]    = vec2((float[2]){1, 0});
        // ui_param.texcoord[5]    = vec2((float[2]){1, 1});
        // ui_param.texid          = 1;
        // ui_param.size           = vec3((float[3]){620 * 2, 372 * 2, 0});
        // struct node_tree *nt1 = game_ui_sprite_alloc(p, &ui_param);
        //
        // ui_param.texcoord[0]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[1]    = vec2((float[2]){0, 1});
        // ui_param.texcoord[2]    = vec2((float[2]){1, 1});
        // ui_param.texcoord[3]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[4]    = vec2((float[2]){1, 0});
        // ui_param.texcoord[5]    = vec2((float[2]){1, 1});
        // ui_param.texid          = 0;
        // ui_param.size           = vec3((float[3]){310, 419, 0});
        // struct node_tree *nt2 = game_ui_sprite_alloc(p, &ui_param);
        //
        // ui_param.texcoord[0]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[1]    = vec2((float[2]){0, 1});
        // ui_param.texcoord[2]    = vec2((float[2]){1, 1});
        // ui_param.texcoord[3]    = vec2((float[2]){0, 0});
        // ui_param.texcoord[4]    = vec2((float[2]){1, 0});
        // ui_param.texcoord[5]    = vec2((float[2]){1, 1});
        // ui_param.texid          = 0;
        // ui_param.size           = vec3((float[3]){310, 419, 0});
        // struct node_tree *nt3 = game_ui_sprite_alloc(p, &ui_param);
        //
        // node_tree_add_node_tree(nt1, nt3);
        // node_tree_add_node_tree(nt1, nt2);
        // node_swap(node_tree_get_node(nt2), node_tree_get_node(nt3));
        //
        // node_tree_set_position(nt3, vec3((float[3]){100, 0, 0}));
        // node_tree_set_position(nt2, vec3((float[3]){50, -50, 0}));
        // node_tree_set_position(nt1, vec3((float[3]){0, 0, 0}));
        //
        // p->ui_root = nt1;
        //
        // /* recalculate color tree */
        // union vec4 v = vec4((float[4]){1, 1, 1, 1});
        // branch_color_traverse(node_tree_get_branch_color(p->ui_root), v);
        //
        // /* recalculate z order  */
        // float z = 0;
        // branch_z_traverse(node_tree_get_branch_z(p->ui_root), &z);

        render_content_set_texture(p->ui_content, 1, texture_alloc_file("res/images/wolf.jpg"));
        render_content_set_texture(p->ui_content, 0, texture_alloc_file("res/images/girl.png"));

        return p;
}

void game_update(struct game *p)
{
        // camera_rotate_around(p->game_cam, quat_angle_axis(DEG_TO_RAD(1), (float[3]){1, 0, 0}));

        branch_transform_queue_traverse(p->update_queue);
        branch_transform_queue_traverse(p->n3d_update_queue);
}

void game_render(struct game *p)
{
        /* process all renderers */
        struct list_head *head;
        list_for_each(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_render(renderer, p->frame);
        }
        /* increase frame by 1 */
        p->frame++;
        p->frame %= BUFFERS;
}

void game_resize(struct game *p, int width, int height)
{
        video_width = width;
        video_height = height;

        union mat4 project = mat4_new_ortho(-video_width/2,
                video_width/2, -video_height/2, video_height/2, 1, 10);
        shader_uniform_update(p->ui_projection_uniform, project.m, sizeof(project));
}

void game_free(struct game *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->node_tree_list) {
                struct node_tree *nt = (struct node_tree *)
                        ((void *)head - offsetof(struct node_tree, life_head));
                node_tree_free(nt);
        }
        list_while_not_singular(head, &p->node_3d_color_list) {
                struct node_3d_color *n = (struct node_3d_color *)
                        ((void *)head - offsetof(struct node_3d_color, life_head));
                node_3d_color_free(n);
        }
        list_while_not_singular(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_free(renderer);
        }
        branch_transform_queue_free(p->update_queue);
        branch_transform_queue_free(p->n3d_update_queue);
        point_light_free(p->world_light);
        camera_free(p->ui_cam);
        camera_free(p->game_cam);
        sfree(p);
}
