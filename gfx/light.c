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
#include <cherry/graphic/light/light.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>

struct direction_light *direction_light_alloc()
{
	struct direction_light *ret    = smalloc(sizeof(struct direction_light));
	ret->u_direction               = shader_uniform_alloc();
	ret->u_ambient                 = shader_uniform_alloc();
	ret->u_diffuse                 = shader_uniform_alloc();
	ret->u_specular                = shader_uniform_alloc();
	ret->u_direction->ref++;
	ret->u_ambient->ref++;
	ret->u_diffuse->ref++;
	ret->u_specular->ref++;
	return ret;
}

void direction_light_free(struct direction_light *p)
{
	shader_uniform_free(p->u_direction);
	shader_uniform_free(p->u_ambient);
	shader_uniform_free(p->u_diffuse);
	shader_uniform_free(p->u_specular);
	sfree(p);
}

void direction_light_set_direction(struct direction_light *p, union vec3 v)
{
	shader_uniform_update(p->u_direction, v.v, sizeof(union vec3));
}

void direction_light_set_ambient(struct direction_light *p, union vec3 v)
{
	shader_uniform_update(p->u_ambient, v.v, sizeof(union vec3));
}

void direction_light_set_diffuse(struct direction_light *p, union vec3 v)
{
	shader_uniform_update(p->u_diffuse, v.v, sizeof(union vec3));
}

void direction_light_set_specular(struct direction_light *p, union vec3 v)
{
	shader_uniform_update(p->u_specular, v.v, sizeof(union vec3));
}

/*
	POINT LIGHT
*/

struct point_light *point_light_alloc()
{
	struct point_light *ret = smalloc(sizeof(struct point_light));
	ret->u_position = shader_uniform_alloc();
	ret->u_constant = shader_uniform_alloc();
	ret->u_linear = shader_uniform_alloc();
	ret->u_quadratic = shader_uniform_alloc();
	ret->u_ambient = shader_uniform_alloc();
	ret->u_diffuse = shader_uniform_alloc();
	ret->u_specular = shader_uniform_alloc();
	ret->u_position->ref++;
	ret->u_constant->ref++;
	ret->u_linear->ref++;
	ret->u_quadratic->ref++;
	ret->u_ambient->ref++;
	ret->u_diffuse->ref++;
	ret->u_specular->ref++;
	return ret;
}

void point_light_free(struct point_light *p)
{
	shader_uniform_free(p->u_position);
	shader_uniform_free(p->u_constant);
	shader_uniform_free(p->u_linear);
	shader_uniform_free(p->u_quadratic);
	shader_uniform_free(p->u_ambient);
	shader_uniform_free(p->u_diffuse);
	shader_uniform_free(p->u_specular);
	sfree(p);
}
void point_light_set_position(struct point_light *p, union vec3 v)
{
	shader_uniform_update(p->u_position, v.v, sizeof(union vec3));
}

void point_light_set_constant(struct point_light *p, float f)
{
	shader_uniform_update(p->u_constant, &f, sizeof(float));
}

void point_light_set_linear(struct point_light *p, float f)
{
	shader_uniform_update(p->u_linear, &f, sizeof(float));
}

void point_light_set_quadratic(struct point_light *p, float f)
{
	shader_uniform_update(p->u_quadratic, &f, sizeof(float));
}

void point_light_set_ambient(struct point_light *p, union vec3 v)
{
	shader_uniform_update(p->u_ambient, v.v, sizeof(union vec3));
}

void point_light_set_diffuse(struct point_light *p, union vec3 v)
{
	shader_uniform_update(p->u_diffuse, v.v, sizeof(union vec3));
}

void point_light_set_specular(struct point_light *p, union vec3 v)
{
	shader_uniform_update(p->u_specular, v.v, sizeof(union vec3));
}

/*
	SPOT LIGHT
*/

struct spot_light *spot_light_alloc()
{
	struct spot_light *ret = smalloc(sizeof(struct spot_light));
	ret->u_position = shader_uniform_alloc();
	ret->u_direction = shader_uniform_alloc();
	ret->u_cut_off = shader_uniform_alloc();
	ret->u_outer_cut_off = shader_uniform_alloc();
	ret->u_constant = shader_uniform_alloc();
	ret->u_linear = shader_uniform_alloc();
	ret->u_quadratic = shader_uniform_alloc();
	ret->u_ambient = shader_uniform_alloc();
	ret->u_diffuse = shader_uniform_alloc();
	ret->u_specular = shader_uniform_alloc();
	ret->u_position->ref++;
	ret->u_direction->ref++;
	ret->u_cut_off->ref++;
	ret->u_outer_cut_off->ref++;
	ret->u_constant->ref++;
	ret->u_linear->ref++;
	ret->u_quadratic->ref++;
	ret->u_ambient->ref++;
	ret->u_diffuse->ref++;
	ret->u_specular->ref++;
	return ret;
}

void spot_light_free(struct spot_light *p)
{
	shader_uniform_free(p->u_position);
	shader_uniform_free(p->u_direction);
	shader_uniform_free(p->u_cut_off);
	shader_uniform_free(p->u_outer_cut_off);
	shader_uniform_free(p->u_constant);
	shader_uniform_free(p->u_linear);
	shader_uniform_free(p->u_quadratic);
	shader_uniform_free(p->u_ambient);
	shader_uniform_free(p->u_diffuse);
	shader_uniform_free(p->u_specular);
	sfree(p);
}

void spot_light_set_position(struct spot_light *p, union vec3 v)
{
	shader_uniform_update(p->u_position, v.v, sizeof(union vec3));
}

void spot_light_set_direction(struct spot_light *p, union vec3 v)
{
	shader_uniform_update(p->u_direction, v.v, sizeof(union vec3));
}

void spot_light_set_cut_off(struct spot_light *p, float f)
{
	shader_uniform_update(p->u_cut_off, &f, sizeof(float));
}

void spot_light_set_outer_cut_off(struct spot_light *p, float f)
{
	shader_uniform_update(p->u_outer_cut_off, &f, sizeof(float));
}

void spot_light_set_constant(struct spot_light *p, float f)
{
	shader_uniform_update(p->u_constant, &f, sizeof(float));
}

void spot_light_set_linear(struct spot_light *p, float f)
{
	shader_uniform_update(p->u_linear, &f, sizeof(float));
}

void spot_light_set_quadratic(struct spot_light *p, float f)
{
	shader_uniform_update(p->u_quadratic, &f, sizeof(float));
}

void spot_light_set_ambient(struct spot_light *p, union vec3 v)
{
	shader_uniform_update(p->u_ambient, v.v, sizeof(union vec3));
}

void spot_light_set_diffuse(struct spot_light *p, union vec3 v)
{
	shader_uniform_update(p->u_diffuse, v.v, sizeof(union vec3));
}

void spot_light_set_specular(struct spot_light *p, union vec3 v)
{
	shader_uniform_update(p->u_specular, v.v, sizeof(union vec3));
}
