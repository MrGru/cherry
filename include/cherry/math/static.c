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
#include <cherry/math/math.h>

union mat4 mat4_identity = (union mat4){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0 ,0 ,0, 1
};

union mat3 mat3_identity = (union mat3) {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
};

union vec4 quat_identity = (union vec4) {
        0, 0, 0, 1
};
