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
#include <cherry/graphic/graphic.h>
#include <cherry/math/math.h>

int video_width                 = 800;
int video_height                = 480;
int shadow_width                = 1024;

float pack_rgb_to_float(int r, int g, int b)
{
        r = MAX(MIN(r - 128, 127), -127);
        g = MAX(MIN(g - 128, 127), -127);
        b = MAX(MIN(b - 128, 127), -127);
        return (float)((r << 16) + (g << 8) + (b));
}
