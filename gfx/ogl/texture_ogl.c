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
#include <cherry/graphic/texture.h>

#if GFX == OGL
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/string.h>
#include <cherry/graphic/image.h>

/*
 * texture cache
 */
static struct map *texture_cache = NULL;

static struct texture *bindings[8] = {
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

static void texture_cache_dispose()
{
        map_deep_free(texture_cache, struct texture *, texture_free);
        texture_cache = NULL;

        int i;
        for_i(i, sizeof(bindings) / sizeof(struct texture *)) {
                bindings[i] = NULL;
        }
}

static void texture_cache_setup()
{
        if(!texture_cache) {
                texture_cache = map_alloc(sizeof(struct texture *));
                cache_add(texture_cache_dispose);
        }
}

/*
 * get texture from cache by file
 */
struct texture *texture_alloc_file(char *file)
{
        texture_cache_setup();

        /* try get texture from cache */
        struct texture *p = map_get(texture_cache, struct texture *, qckey(file));
        if(p) goto finish;

        /* allocate new texture*/
        struct image *img = image_alloc(file);
        p = texture_alloc_image(img);
        image_free(img);

        /* add texture to cache and retain */
        map_set(texture_cache, qckey(file), &p);
        p->ref++;
finish:;
        return p;
}

/*
 * allocate new texture from image
 */
struct texture *texture_alloc_image(struct image *p)
{
        texture_cache_setup();

        i16 bid;
        struct texture *t = bindings[0];

        /* allocate texture and data */
        struct texture *r = smalloc(sizeof(struct texture));
        glGenTextures(1, &r->id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, r->id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width(p),
                image_height(p), 0, image_type(p), GL_UNSIGNED_BYTE, image_pixels(p));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        r->width        = image_width(p);
        r->height       = image_height(p);
        r->ref          = 0;
        r->active_id    = -1;

        /* rebind previous texture */
        if(t) glBindTexture(GL_TEXTURE_2D, t->id);
        return r;
}

/*
 * allocate new depth texture
 */
struct texture *texture_alloc_depth(u16 width, u16 height)
{
        texture_cache_setup();

        i16 bid;
        struct texture *t = bindings[0];
        /* allocate texture and data */

        struct texture *r = smalloc(sizeof(struct texture));
        glGenTextures(1, &r->id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, r->id);

        if(depth_texture_enable) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                        width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
        } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        r->width        = width;
        r->height       = height;
        r->ref          = 0;
        r->active_id    = -1;

        /* rebind previous texture */
        if(t) glBindTexture(GL_TEXTURE_2D, t->id);
        return r;
}

/*
 * decrease texture's ref by 1
 * if the texture's ref equal to zero then deallocate it
 */
void texture_free(struct texture *p)
{
        p->ref--;
        if(p->ref <= 0) {
                if(p->active_id >= 0) {
                        bindings[p->active_id] = NULL;
                }
                glDeleteTextures(1, &p->id);
                sfree(p);
        }
}

/*
 * bind texture
 */
void texture_bind(struct texture *p, u32 active_id)
{
        if(p->active_id == active_id) return;

        /* if texture is already binded, set it's priority is highest */
        if(p->active_id >= 0)
        {
                bindings[p->active_id] = NULL;
        }

        if(bindings[active_id]) {
                bindings[active_id]->active_id = -1;
        }

        p->active_id = active_id;
        glActiveTexture(GL_TEXTURE0 + active_id);
        glBindTexture(GL_TEXTURE_2D, p->id);
        bindings[active_id] = p;
}

/*
 * deallocate unused textures
 */
void texture_remove_unused()
{
        if(!texture_cache) return;

        struct array *keys = array_alloc(sizeof(struct bytes *), UNORDERED);
        map_get_list_key(texture_cache, keys);
        struct bytes **key;
        array_for_each(key, keys) {
                struct texture *p = map_get(texture_cache, struct texture *, qskey(*key));
                /*
                 * deallocate texture only has reference to cache
                 */
                if(p->ref == 1) {
                        map_remove_key(texture_cache, qskey(*key));
                        texture_free(p);
                }
        }
        array_free(keys);
}

#endif
