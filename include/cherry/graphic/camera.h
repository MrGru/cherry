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
#ifndef __CHERRY_GRAPHIC_CAMERA_H__
#define __CHERRY_GRAPHIC_CAMERA_H__

#include <cherry/graphic/types.h>

struct camera *camera_alloc(union mat4 project, union mat4 lookat);

void camera_free(struct camera *p);

void camera_set_project(struct camera *p, union mat4 project);

void camera_move_around(struct camera *p, union vec3 offset);

void camera_move_forward(struct camera *p, float length);

void camera_rotate_self(struct camera *p, union vec4 quat);

void camera_rotate_around(struct camera *p, union vec4 quat);

union vec3 camera_get_position(struct camera *p);

union vec3 camera_get_direction(struct camera *p);

#endif
