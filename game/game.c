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
#include <game.h>
#include <game_setup.h>
#include <cherry/list.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/graphic/node_manager.h>
#include <cherry/graphic/camera.h>
#include <cherry/graphic/node.h>
#include <cherry/graphic/texture.h>

struct node *test_node;

struct game *game_alloc()
{
        struct game *p = smalloc(sizeof(struct game));
        game_setup(p);

        p->frame = 0;

        test_node = node_alloc(p->manager_game);
        node_show_spine(test_node, SHADER_2D_TEXTURE_COLOR, "res/spine/raptor.skel", "res/spine/raptor.atlas", 0.005);
        node_set_size(test_node, (union vec3){100, 100, 0});

        node_spin_set_animation(test_node, 0, "walk", 1);
        node_spin_add_animation(test_node, 1, "gungrab", 0, 2);

        // node_set_rotation(test_node, quat_angle_axis(DEG_TO_RAD(-45), (float[3]){0, 0, 1}));
        node_set_position(test_node, (union vec3){100, 100, 0});
        node_set_origin(test_node, (union vec3){0, 0, 0});
        // node_set_scale(test_node, (union vec3){2, 2, 2});
        // {
        //         struct node *child = node_alloc(p->manager_game);
        //         node_show_sprite(child, SHADER_2D_TEXTURE_COLOR, texture_alloc_file("res/images/gradient_2.jpg"));
        //         node_set_size(child, (union vec3){100, 100, 0});
        //         node_set_position(child, (union vec3){50, 50, 0});
        //         node_set_origin(child, (union vec3){0, 0, 0});
        //         node_set_rotation(child, quat_angle_axis(DEG_TO_RAD(45), (float[3]){0, 0, 1}));
        //         node_add_child(test_node, child);
        // }
        // {
        //         struct node *child = node_alloc(p->manager_game);
        //         node_show_sprite(child, SHADER_2D_TEXTURE_COLOR, texture_alloc_file("res/images/gradient_2.jpg"));
        //         node_show_sprite(child, SHADER_2D_TEXTURE_COLOR, texture_alloc_file("res/images/gradient_1.jpg"));
        //         node_set_size(child, (union vec3){100, 100, 0});
        //         node_set_position(child, (union vec3){50, 50, 0});
        //         node_set_origin(child, (union vec3){0, 0, 0});
        //         node_set_rotation(child, quat_angle_axis(DEG_TO_RAD(-60), (float[3]){0, 0, 1}));
        //         node_add_child(test_node, child);
        // }
        return p;
}

void game_update(struct game *p)
{
        node_update_spine(test_node, 1.0f / 60, p->frame);
        int i;
        struct node_manager **manager    = &p->manager[0];
        for_i(i, sizeof(p->manager) / sizeof(p->manager[0])) {
                node_manager_update_transform(*manager);
                manager++;
        }
}

void game_render(struct game *p)
{
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        node_render(test_node, p->frame);

        p->frame++;
        if(p->frame == BUFFERS) p->frame = 0;
}

void game_read_event(struct game *p, struct event *e)
{

}

void game_free(struct game *p)
{
        int i;
        struct node_manager **manager    = &p->manager[0];
        for_i(i, sizeof(p->manager) / sizeof(p->manager[0])) {
                node_manager_free(*manager);
                manager++;
        }

        struct camera **camera           = &p->camera[0];
        for_i(i, sizeof(p->camera) / sizeof(p->camera[0])) {
                camera_free(*camera);
                camera++;
        }
        sfree(p);
}

void game_resize(struct game *p, int width, int height)
{

}
