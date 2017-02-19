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

struct node_tree *game_ui_sprite_alloc(struct game *p, struct ui_sprite_param *param)
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
        u32 triangles_per_object = 100;
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
                                buffer_vertex_alloc(instances * triangles_per_object, BUFFER_PINNED)
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

struct node_3d_color *game_n3d_color_alloc(struct game *p, struct n3d_color_param *param)
{
        struct node_3d_color *n = node_3d_color_alloc(node_alloc(p->game_content));
        node_3d_color_set_branch_transform(n, branch_transform_alloc(1, p->n3d_update_queue));
        node_3d_color_set_branch_color(n, branch_color_alloc(2));
        u8 vbid[3] = {3, 4, 5};
        node_3d_color_set_twig_3d_vertex(n, twig_3d_vertex_alloc(vbid));
        list_add_tail(&n->life_head, &p->node_3d_color_list);

        node_3d_color_fill_vertex(n, param->v1, param->v2, param->v3, param->vlen);

        node_3d_color_set_size(n, param->size);
        node_3d_color_set_position(n, vec3((float[3]){0, 0, 0}));

        return n;
}

struct game *game_alloc()
{
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

        struct shader *s3d_color = shader_3d_color_alloc();
        s3d_color->descriptor = descriptor_3d_color_get(100);
        struct render_queue *s3d_queue = render_queue_alloc(&stage->content_queue_list, s3d_color);
        __game_create_game_content(p, s3d_queue);
        p->game_projection_uniform = shader_uniform_alloc();
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_PROJECT, p->game_projection_uniform);

        p->game_cam = camera_alloc(mat4_new_look_at(
                0, 0, 1000,
                0, 0, 0,
                0, 1, 0
        ));
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_VIEW, p->game_cam->view_uniform);
        union mat4 project = mat4_new_perspective(DEG_TO_RAD(45.0f), video_width * 1.0f / video_height, 1.0f, 10000.0f);
        shader_uniform_update(p->game_projection_uniform, project.m, sizeof(project));

        struct node_3d_color *n1, *n2;
        {
                struct n3d_color_param n3d_param;
                n3d_param.size = vec3((float[3]){100, 100, 1});
                u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
                n3d_param.v1 = smalloc(vsize);
                n3d_param.v2 = smalloc(vsize);
                n3d_param.v3 = smalloc(vsize);
                smemset(n3d_param.v1, 0, vsize);
                smemset(n3d_param.v2, 0, vsize);
                smemset(n3d_param.v3, 0, vsize);
                n3d_param.vlen = vsize;
                n3d_param.v1[0] = vec3((float[3]){-0.5,  0.5, 0});
                n3d_param.v2[0] = vec3((float[3]){-0.5, -0.5, 0});
                n3d_param.v3[0] = vec3((float[3]){ 0.5, -0.5, 0});
                n3d_param.v1[1] = vec3((float[3]){-0.5,  0.5, 0});
                n3d_param.v2[1] = vec3((float[3]){ 0.5,  0.5, 0});
                n3d_param.v3[1] = vec3((float[3]){ 0.5, -0.5, 0});
                struct node_3d_color *n3d = game_n3d_color_alloc(p, &n3d_param);
                sfree(n3d_param.v1);
                sfree(n3d_param.v2);
                sfree(n3d_param.v3);
                n1 = n3d;
        }
        {
                struct n3d_color_param n3d_param;
                n3d_param.size = vec3((float[3]){100, 100, 1});
                u32 vsize = sizeof(union vec3) * p->game_content->instance_multiple;
                n3d_param.v1 = smalloc(vsize);
                n3d_param.v2 = smalloc(vsize);
                n3d_param.v3 = smalloc(vsize);
                smemset(n3d_param.v1, 0, vsize);
                smemset(n3d_param.v2, 0, vsize);
                smemset(n3d_param.v3, 0, vsize);
                n3d_param.vlen = vsize;
                n3d_param.v1[0] = vec3((float[3]){-0.5,  0.5, 0});
                n3d_param.v2[0] = vec3((float[3]){-0.5, -0.5, 0});
                n3d_param.v3[0] = vec3((float[3]){ 0.5, -0.5, 0});
                n3d_param.v1[1] = vec3((float[3]){-0.5,  0.5, 0});
                n3d_param.v2[1] = vec3((float[3]){ 0.5,  0.5, 0});
                n3d_param.v3[1] = vec3((float[3]){ 0.5, -0.5, 0});
                struct node_3d_color *n3d = game_n3d_color_alloc(p, &n3d_param);
                sfree(n3d_param.v1);
                sfree(n3d_param.v2);
                sfree(n3d_param.v3);
                n2 = n3d;
        }
        node_3d_color_add_node_3d_color(n1, n2);

        node_3d_color_set_position(n2, vec3((float[3]){100, 0, 0}));
        node_3d_color_set_color(n2, vec4((float[4]){1, 0, 0, 1}));

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
        // union vec3 *pos = node_tree_get_position(nt2);
        // node_tree_set_position(nt2, vec3_add(*pos, (union vec3){0.1, 0, 0}));

        // camera_move_around(p->cam, (union vec3){0.5, 0, 0});

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
        camera_free(p->ui_cam);
        camera_free(p->game_cam);
        sfree(p);
}
