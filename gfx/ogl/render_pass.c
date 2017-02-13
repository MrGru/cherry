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
#include <cherry/graphic/render.h>

#if GFX == OGL
#include <cherry/memory.h>
#include <cherry/graphic/texture.h>
#include <cherry/stdio.h>

static i32 main_pass_id = -1;

static void store_main_pass()
{
        if(main_pass_id == -1) {
                GLint id;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);
                main_pass_id = (i32)id;
        }
}

static void render_pass_main_free(struct render_pass *p)
{
        struct main_render_pass *r = (struct main_render_pass *)
                ((void *)p - offsetof(struct main_render_pass, pass));
        sfree(r);
}

static void render_pass_shadow_free(struct render_pass *p)
{
        struct shadow_render_pass *r = (struct shadow_render_pass *)
                ((void *)p - offsetof(struct shadow_render_pass, pass));
        texture_free(r->map);
        glDeleteFramebuffers(1, &p->id);
        sfree(r);
}


struct render_pass *render_pass_main_alloc()
{
        store_main_pass();
        struct main_render_pass *p = smalloc(sizeof(struct main_render_pass));
        p->pass.del     = render_pass_main_free;
        p->pass.id      = main_pass_id;
        return &p->pass;
}

struct render_pass *render_pass_shadow_alloc()
{
        store_main_pass();

        /* store current binding framebuffer */
        GLint id;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);

        struct shadow_render_pass *p = smalloc(sizeof(struct shadow_render_pass));
        p->pass.del = render_pass_shadow_free;

        /* allocate depth map */
        glGenFramebuffers(1, &p->pass.id);
        glBindFramebuffer(GL_FRAMEBUFFER, p->pass.id);
        p->map  = texture_alloc_depth(shadow_width, shadow_width);
        p->map->ref++;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, p->map->id, 0);
        // glDrawBuffer(GL_NONE);
        // glReadBuffer(GL_NONE);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
        if(status != GL_FRAMEBUFFER_COMPLETE) {
                debug("failed to make complete framebuffer object %x", status);
        }
        /* rebind previous framebuffer */
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        return &p->pass;
}
#endif
