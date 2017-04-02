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
#include <game_setup.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/graphic/camera.h>
#include <cherry/graphic/uniform.h>
#include <cherry/graphic/node_manager.h>
#include <cherry/graphic/shader/shader_2d_texture_color.h>
#include <cherry/graphic/render_pass.h>
#include <cherry/stdio.h>

static void __setup_game_camera(struct game *p)
{
        union mat4 project      = mat4_new_ortho(
                0, video_width,
                0, video_height,
                0, 2
        );

        union mat4 lookat       = mat4_new_look_at(
                0, 0, 1,
                0, 0, 0,
                0, 1, 0
        );

        p->game_camera          = camera_alloc(project, lookat);
}

static void __setup_hud_camera(struct game *p)
{
        union mat4 project      = mat4_new_ortho(
                0, video_width,
                0, video_height,
                0, 2
        );

        union mat4 lookat       = mat4_new_look_at(
                0, 0, 1,
                0, 0, 0,
                0, 1, 0
        );

        p->hud_camera          = camera_alloc(project, lookat);
}

static void __setup_game_manager(struct game *p)
{
        struct node_manager *m  = node_manager_alloc();

        struct shader *s        = shader_2d_texture_color_get();
        u32 key                 = SHADER_2D_TEXTURE_COLOR;
        map_set(m->shaders, &key, sizeof(key), &s);

        key                     = UNIFORM_CAMERA;
        map_set(m->common_uniform_buffers, &key, sizeof(key), &p->game_camera->buffer);

        p->manager_game         = m;
}

static void __setup_hud_manager(struct game *p)
{
        struct node_manager *m  = node_manager_alloc();

        struct shader *s        = shader_2d_texture_color_get();
        u32 key                 = SHADER_2D_TEXTURE_COLOR;
        map_set(m->shaders, &key, sizeof(key), &s);

        key                     = UNIFORM_CAMERA;
        map_set(m->common_uniform_buffers, &key, sizeof(key), &p->hud_camera->buffer);

        p->manager_hud          = m;
}

static void __setup_main_pass(struct game *p)
{
        p->main_pass            = render_pass_main_alloc();
}

void game_setup(struct game *p)
{
        __setup_main_pass(p);

        __setup_game_camera(p);
        __setup_hud_camera(p);

        __setup_game_manager(p);
        __setup_hud_manager(p);
}
