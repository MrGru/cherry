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
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/shader.h>
#include <cherry/math/math.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/node/node_tree.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>
#include <cherry/list.h>
#include <cherry/graphic/camera.h>

struct node_tree *nt1 = NULL;
struct node_tree *nt2 = NULL;
struct node_tree *nt3 = NULL;
struct shader_uniform *project_uniform = NULL;

struct game *game_alloc()
{
        struct game *p  = smalloc(sizeof(struct game));
        INIT_LIST_HEAD(&p->renderer_list);
        INIT_LIST_HEAD(&p->node_tree_list);
        INIT_LIST_HEAD(&p->update_queue.list);
        p->update_queue.root = NULL;
        p->update_queue.full = 0;
        p->frame        = 0;

        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        renderer_set_color(r, &(union vec4){0.3, 0, 1.0, 1});
        list_add_tail(&r->chain_head, &p->renderer_list);

        struct render_stage *stage = render_stage_alloc(r);

        struct shader *s = shader_texture_alloc();

        struct render_queue *queue = render_queue_alloc(&stage->content_queue_list, s);

        struct array *buffers[BUFFERS];
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_quad_alloc(BUFFER_PINNED)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_z_alloc(900, BUFFER_PINNED)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_transform_alloc(900, BUFFER_PINNED)});
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_color_alloc(900, BUFFER_PINNED)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_texid_alloc(900, BUFFER_PINNED)
                });
                u8 j;
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_texcoord_alloc(900, BUFFER_PINNED)
                        });
                }
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_vertex_alloc(900, BUFFER_PINNED)
                        });
                }
        }

        struct render_content *content = render_content_alloc(queue, buffers, 6, 900);

        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        project_uniform  = shader_uniform_alloc();
        shader_set_uniform(s, SHADER_COLOR_PROJECT, project_uniform);

        union mat4 project = mat4_new_ortho(-video_width/2,
                video_width/2, -video_height/2, video_height/2, 1, 10);
        p->cam = camera_alloc(mat4_new_look_at(
                0, 0, 1,
                0, 0, 0,
                0, 1, 0
        ));
        shader_set_uniform(s, SHADER_COLOR_VIEW, p->cam->view_uniform);
        shader_uniform_update(project_uniform, project.m, sizeof(project));

        {
                nt1 = node_tree_alloc(node_alloc(content));
                node_tree_set_branch_z(nt1, branch_z_alloc(1));
                node_tree_set_branch_transform(nt1, branch_transform_alloc(2, &p->update_queue));
                node_tree_set_branch_color(nt1, branch_color_alloc(3));
                node_tree_set_twig_texid(nt1, twig_texid_alloc(4));
                u8 cbid[3] = {5, 6, 7};
                node_tree_set_twig_texcoord(nt1, twig_texcoord_alloc(cbid));
                u8 vbid[3] = {8, 9, 10};
                node_tree_set_twig_vertex(nt1, twig_vertex_alloc(vbid));
                list_add_tail(&nt1->life_head, &p->node_tree_list);

                node_tree_set_texcoord(nt1, 0, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt1, 1, vec2((float[2]){0, 1}), 0);
                node_tree_set_texcoord(nt1, 2, vec2((float[2]){1, 1}), 0);
                node_tree_set_texcoord(nt1, 3, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt1, 4, vec2((float[2]){1, 0}), 0);
                node_tree_set_texcoord(nt1, 5, vec2((float[2]){1, 1}), 1);

                node_tree_set_vertex(nt1, 0, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt1, 1, vec2((float[2]){-0.5, -0.5}), 0);
                node_tree_set_vertex(nt1, 2, vec2((float[2]){ 0.5, -0.5}), 0);
                node_tree_set_vertex(nt1, 3, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt1, 4, vec2((float[2]){ 0.5,  0.5}), 0);
                node_tree_set_vertex(nt1, 5, vec2((float[2]){ 0.5, -0.5}), 1);
        }
        {
                nt2 = node_tree_alloc(node_alloc(content));
                node_tree_set_branch_z(nt2, branch_z_alloc(1));
                node_tree_set_branch_transform(nt2, branch_transform_alloc(2, &p->update_queue));
                node_tree_set_branch_color(nt2, branch_color_alloc(3));
                node_tree_set_twig_texid(nt2, twig_texid_alloc(4));
                u8 cbid[3] = {5, 6, 7};
                node_tree_set_twig_texcoord(nt2, twig_texcoord_alloc(cbid));
                u8 vbid[3] = {8, 9, 10};
                node_tree_set_twig_vertex(nt2, twig_vertex_alloc(vbid));
                list_add_tail(&nt2->life_head, &p->node_tree_list);

                node_tree_set_texcoord(nt2, 0, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt2, 1, vec2((float[2]){0, 1}), 0);
                node_tree_set_texcoord(nt2, 2, vec2((float[2]){1, 1}), 0);
                node_tree_set_texcoord(nt2, 3, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt2, 4, vec2((float[2]){1, 0}), 0);
                node_tree_set_texcoord(nt2, 5, vec2((float[2]){1, 1}), 1);

                node_tree_set_vertex(nt2, 0, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt2, 1, vec2((float[2]){-0.5, -0.5}), 0);
                node_tree_set_vertex(nt2, 2, vec2((float[2]){ 0.5, -0.5}), 0);
                node_tree_set_vertex(nt2, 3, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt2, 4, vec2((float[2]){ 0.5,  0.5}), 0);
                node_tree_set_vertex(nt2, 5, vec2((float[2]){ 0.5, -0.5}), 1);
        }
        {
                nt3 = node_tree_alloc(node_alloc(content));
                node_tree_set_branch_z(nt3, branch_z_alloc(1));
                node_tree_set_branch_transform(nt3, branch_transform_alloc(2, &p->update_queue));
                node_tree_set_branch_color(nt3, branch_color_alloc(3));
                node_tree_set_twig_texid(nt3, twig_texid_alloc(4));
                u8 cbid[3] = {5, 6, 7};
                node_tree_set_twig_texcoord(nt3, twig_texcoord_alloc(cbid));
                u8 vbid[3] = {8, 9, 10};
                node_tree_set_twig_vertex(nt3, twig_vertex_alloc(vbid));
                list_add_tail(&nt3->life_head, &p->node_tree_list);
                node_tree_set_texcoord(nt3, 0, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt3, 1, vec2((float[2]){0, 1}), 0);
                node_tree_set_texcoord(nt3, 2, vec2((float[2]){1, 1}), 0);
                node_tree_set_texcoord(nt3, 3, vec2((float[2]){0, 0}), 0);
                node_tree_set_texcoord(nt3, 4, vec2((float[2]){1, 0}), 0);
                node_tree_set_texcoord(nt3, 5, vec2((float[2]){1, 1}), 1);

                node_tree_set_vertex(nt3, 0, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt3, 1, vec2((float[2]){-0.5, -0.5}), 0);
                node_tree_set_vertex(nt3, 2, vec2((float[2]){ 0.5, -0.5}), 0);
                node_tree_set_vertex(nt3, 3, vec2((float[2]){-0.5,  0.5}), 0);
                node_tree_set_vertex(nt3, 4, vec2((float[2]){ 0.5,  0.5}), 0);
                node_tree_set_vertex(nt3, 5, vec2((float[2]){ 0.5, -0.5}), 1);
        }

        node_tree_add_node_tree(nt1, nt2);
        node_tree_add_node_tree(nt1, nt3);

        node_tree_set_texid(nt1, 1);
        node_tree_set_texid(nt2, 0);
        node_tree_set_texid(nt3, 0);
        node_tree_set_size(nt1, vec3((float[3]){620 * 2, 372 * 2, 0}));
        node_tree_set_size(nt2, vec3((float[3]){523, 391, 0}));
        node_tree_set_size(nt3, vec3((float[3]){523, 391, 0}));

        node_tree_set_position(nt3, vec3((float[3]){100, 0, 0}));
        node_tree_set_position(nt2, vec3((float[3]){-100, -100, 0}));
        node_tree_set_color(nt2, vec4((float[4]){1, 0, 1, 0.5}));

        /* recalculate transform */
        union mat4 m = mat4_identity;
        branch_transform_traverse(node_tree_get_branch_transform(nt1), m);

        /* recalculate color tree */
        union vec4 v = vec4((float[4]){1, 1, 1, 1});
        branch_color_traverse(node_tree_get_branch_color(nt1), v);

        /* recalculate z order  */
        float z = 0;
        branch_z_traverse(node_tree_get_branch_z(nt1), &z);

        render_content_set_texture(content, 1, texture_alloc_file("res/images/wolf.jpg"));
        render_content_set_texture(content, 0, texture_alloc_file("res/images/text.png"));

        return p;
}

void game_update(struct game *p)
{
        union vec3 *pos = node_tree_get_position(nt2);
        node_tree_set_position(nt2, vec3_add(*pos, (union vec3){1, 0, 0}));

        pos = node_tree_get_position(nt1);
        node_tree_set_position(nt1, vec3_add(*pos, (union vec3){1, 0, 0}));

        if(p->update_queue.full) {
                union mat4 m = mat4_identity;
                branch_transform_traverse(p->update_queue.root, m);
                p->update_queue.root = NULL;
                p->update_queue.full = 0;
        } else {
                struct list_head *head, *next;
                list_for_each_safe(head, next, &p->update_queue.list) {
                        struct branch_transform *b = (struct branch_transform *)
                                ((void *)head - offsetof(struct branch_transform, update_queue_head));
                        struct list_head *bhead, *bnext;
                        list_for_each_safe(bhead, bnext, &b->child_updater_list) {
                                struct branch_transform *bc = (struct branch_transform *)
                                        ((void *)bhead - offsetof(struct branch_transform, updater_head));
                                branch_transform_traverse(bc, b->last_transform);
                        }
                        INIT_LIST_HEAD(&b->child_updater_list);
                        list_del(head);
                }
        }
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
        shader_uniform_update(project_uniform, project.m, sizeof(project));
}

void game_free(struct game *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->node_tree_list) {
                struct node_tree *nt = (struct node_tree *)
                        ((void *)head - offsetof(struct node_tree, life_head));
                node_tree_free(nt);
        }
        list_while_not_singular(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_free(renderer);
        }
        camera_free(p->cam);
        sfree(p);
}
