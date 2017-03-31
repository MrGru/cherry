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
#include <cherry/graphic/shader/shader_2d_texture_color.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/shader.h>

/*
 * setup for opengl branch
 */
#if GFX == OGL

#if OS == WEB
static inline struct string *get_vert()
{
        return shader_read_file("res/shaders/shader_2d_texture_color_web.vert");
}

static inline struct string *get_frag()
{
        return shader_read_file("res/shaders/shader_2d_texture_color_web.frag");
}
#else
static inline struct string *get_vert()
{
        return shader_read_file("res/shaders/gles2/shader_2d_texture_color.vert");
}

static inline struct string *get_frag()
{
        return shader_read_file("res/shaders/gles2/shader_2d_texture_color.frag");
}
#endif

/*
 * setup for metal branch
 */
#elif GFX == MTL

static inline struct string *get_vert()
{
        return string_alloc_chars("vertex_2d_texture_color", sizeof("vertex_2d_texture_color")-1);
}

static inline struct string *get_frag()
{
        return string_alloc_chars("fragment_2d_texture_color", sizeof("fragment_2d_texture_color")-1);
}

#endif

static struct shader *instance = NULL;

static void free_cache()
{
        if(instance) {
                shader_free(instance);
                instance = NULL;
        }
}

#if GFX == OGL

static void __add_link(struct shader *p, struct link_uniform *link, char *name)
{
        i32 id  = glGetUniformLocation(p->id, name);
        array_push(link->ids, &id);
}

/*
 * link has to be setup in order carefully
 */
static void __setup_link_uniform(struct shader *p)
{
        struct link_uniform *link = link_uniform_alloc();
        array_push(p->link_uniforms, &link);
        __add_link(p, link, "camera.project");
        __add_link(p, link, "camera.view");
        __add_link(p, link, "camera.position");

        link = link_uniform_alloc();
        array_push(p->link_uniforms, &link);
        __add_link(p, link, "transform.transform");
}
#endif

struct shader *shader_2d_texture_color_get()
{
        if(instance) goto response;
create:;
        cache_add(free_cache);
        struct string *vert     = get_vert();
        struct string *frag     = get_frag();
        instance                = shader_alloc(vert->ptr, frag->ptr);
        string_free(vert);
        string_free(frag);
        #if GFX == OGL
        __setup_link_uniform(instance);
        #endif
response:;
        return instance;
}
