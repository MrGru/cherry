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
#include <cherry/graphic/render_pass.h>

#if GFX == OGL
#include <cherry/memory.h>
#include <cherry/graphic/texture.h>

static i32 main_pass_id = -1;

static void __store_main_pass()
{
        if(main_pass_id == -1) {
                GLint id;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);
                main_pass_id = (i32)id;
        }
}


struct render_pass *render_pass_main_alloc()
{
        __store_main_pass();
        struct render_pass *p   = smalloc(sizeof(struct render_pass));
        p->id                   = main_pass_id;
        p->data                 = NULL;
        p->frame                = -1;
        return p;
}

struct render_pass *render_pass_depth_alloc()
{
        // need implement
}

void render_pass_free(struct render_pass *p)
{
        if(p->data) p->data_free(p->data);
        sfree(p);
}


#endif
