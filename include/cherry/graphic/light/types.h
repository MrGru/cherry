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
#ifndef __CHERRY_GRAPHIC_LIGHT_TYPES_H__
#define __CHERRY_GRAPHIC_LIGHT_TYPES_H__

#include <cherry/graphic/types.h>

struct direction_light {
	struct shader_uniform *u_direction;
	struct shader_uniform *u_ambient;
	struct shader_uniform *u_diffuse;
	struct shader_uniform *u_specular;
};

struct point_light {
        struct shader_uniform *u_position;
	struct shader_uniform *u_constant;
	struct shader_uniform *u_linear;
	struct shader_uniform *u_quadratic;
	struct shader_uniform *u_ambient;
	struct shader_uniform *u_diffuse;
	struct shader_uniform *u_specular;
};

struct spot_light {
	struct shader_uniform *u_position;
	struct shader_uniform *u_direction;
	struct shader_uniform *u_cut_off;
	struct shader_uniform *u_outer_cut_off;
	struct shader_uniform *u_constant;
	struct shader_uniform *u_linear;
	struct shader_uniform *u_quadratic;
	struct shader_uniform *u_ambient;
	struct shader_uniform *u_diffuse;
	struct shader_uniform *u_specular;
};

#endif
