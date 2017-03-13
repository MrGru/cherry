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
#import <cherry/graphic/render.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/graphic/texture.h>
#import <cherry/stdio.h>

static void render_pass_main_free(struct render_pass *p)
{
        struct main_render_pass *r = (struct main_render_pass *)
                ((void *)p - offsetof(struct main_render_pass, pass));
        p->ptr = NULL;
        sfree(r);
}

struct render_pass *render_pass_main_alloc()
{
        struct main_render_pass *p = smalloc(sizeof(struct main_render_pass));
        p->pass.del     = render_pass_main_free;
        p->pass.ptr     = (__bridge void *)(shared_mtl_main_pass);
        return &p->pass;
}

struct render_pass *render_pass_shadow_alloc()
{
        return NULL;
}
#endif
