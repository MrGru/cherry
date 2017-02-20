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
#ifndef __CHERRY_GRAPHIC_LIGHT_LIGHT_H__
#define __CHERRY_GRAPHIC_LIGHT_LIGHT_H__

#include <cherry/graphic/light/types.h>
#include <cherry/math/types.h>

/*
 * direction light definitions
 */
struct direction_light *direction_light_alloc();
void direction_light_free(struct direction_light *p);
void direction_light_set_direction(struct direction_light *p, union vec3 v);
void direction_light_set_ambient(struct direction_light *p, union vec3 v);
void direction_light_set_diffuse(struct direction_light *p, union vec3 v);
void direction_light_set_specular(struct direction_light *p, union vec3 v);

/*
 * point light definitions
 */
struct point_light *point_light_alloc();
void point_light_free(struct point_light *p);
void point_light_set_position(struct point_light *p, union vec3 v);
void point_light_set_constant(struct point_light *p, float f);
void point_light_set_linear(struct point_light *p, float f);
void point_light_set_quadratic(struct point_light *p, float f);
void point_light_set_ambient(struct point_light *p, union vec3 v);
void point_light_set_diffuse(struct point_light *p, union vec3 v);
void point_light_set_specular(struct point_light *p, union vec3 v);

/*
 * spot light definitions
 */
struct spot_light *spot_light_alloc();
void spot_light_free(struct spot_light *p);
void spot_light_set_position(struct spot_light *p, union vec3 v);
void spot_light_set_direction(struct spot_light *p, union vec3 v);
void spot_light_set_cut_off(struct spot_light *p, float f);
void spot_light_set_outer_cut_off(struct spot_light *p, float f);
void spot_light_set_constant(struct spot_light *p, float f);
void spot_light_set_linear(struct spot_light *p, float f);
void spot_light_set_quadratic(struct spot_light *p, float f);
void spot_light_set_ambient(struct spot_light *p, union vec3 v);
void spot_light_set_diffuse(struct spot_light *p, union vec3 v);
void spot_light_set_specular(struct spot_light *p, union vec3 v);

#endif
