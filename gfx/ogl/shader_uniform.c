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
#include <cherry/graphic/shader.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/stdio.h>

/*
 * allocate new shader uniform tracker
 */
struct shader_uniform_track *shader_uniform_track_alloc()
{
        struct shader_uniform_track *p = smalloc(sizeof(struct shader_uniform_track));
        p->uniform      = NULL;
        p->pipeline     = NULL;
        p->name         = string_alloc(0);
        p->id           = -1;
        return p;
}

/*
 * deallocate uniform tracker p
 */
void shader_uniform_track_free(struct shader_uniform_track *p)
{
        if(p->uniform) shader_uniform_free(p->uniform);
        string_free(p->name);
        sfree(p);
}

#endif
