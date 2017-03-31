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
#include <cherry/graphic/camera.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>

static inline void __camera_update_position(struct camera *p)
{
        if(p->position_update) {
                p->position_update              = 0;
                struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
                union mat4 inv                  = mat4_invert(u->view);
                union vec3 pos                  = vec3((float[3]){inv.m[12], inv.m[13], inv.m[14]});
                uniform_buffer_set(p->buffer, CAMERA_UNIFORM_POSITION, &pos, sizeof(pos));
        }
}

static inline void __camera_update(struct camera *p)
{
        p->position_update = 1;
        __camera_update_position(p);
}

struct camera *camera_alloc(union mat4 project, union mat4 lookat)
{
        struct camera *p        = smalloc(sizeof(struct camera));
        p->buffer               = camera_uniform_buffer_alloc();
        p->position_update      = 1;

        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_PROJECT, &project, sizeof(project));
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_VIEW, &lookat, sizeof(lookat));
        __camera_update(p);
        return p;
}

void camera_free(struct camera *p)
{
        sfree(p);
}

void camera_set_project(struct camera *p, union mat4 project)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_PROJECT, &project, sizeof(project));
}

void camera_move_around(struct camera *p, union vec3 offset)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        union mat4 view                 = mat4_translate(u->view, vec3_neg(offset));
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_VIEW, &view, sizeof(view));
        __camera_update(p);
}

void camera_move_forward(struct camera *p, float length)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        union mat4 view                 = mat4_translate(u->view, vec3_neg(vec3_mul_scalar(camera_get_direction(p), length)));
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_VIEW, &view, sizeof(view));
        __camera_update(p);
}

void camera_rotate_self(struct camera *p, union vec4 quat)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        union mat4 view                 = mat4_mul(mat4_from_quat(quat), u->view);
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_VIEW, &view, sizeof(view));
        __camera_update(p);
}

void camera_rotate_around(struct camera *p, union vec4 quat)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        union mat4 view                 = mat4_mul(u->view, mat4_from_quat(quat));
        uniform_buffer_set(p->buffer, CAMERA_UNIFORM_VIEW, &view, sizeof(view));
        __camera_update(p);
}

union vec3 camera_get_position(struct camera *p)
{
        __camera_update_position(p);
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        return u->position;
}

union vec3 camera_get_direction(struct camera *p)
{
        struct camera_uniform *u        = (struct camera_uniform *) p->buffer->data;
        return vec3_normalize(vec3((float[3]){-u->view.m[2], -u->view.m[6], -u->view.m[10]}));
}
