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

/*
 * priority queue to maintain texture binding chain
 * application may bind several textures for a render pass so
 * for a render cycle :
 * - if number of textures is smaller than max_texture_units then
 * priority queue can prevent rebind textures
 * - if number of textures is greater than max_texture_units then
 * priority queue can maintain binding chain
 */
static struct list_head bind_list = LIST_HEAD_INIT(bind_list);
static struct array *remain_binds = NULL;

static void texture_cache_dispose()
{
        map_deep_free(texture_cache, struct texture *, texture_free);
        array_free(remain_binds);
        texture_cache = NULL;
        remain_binds = NULL;
}

static void texture_cache_setup()
{
        if(!texture_cache) {
                texture_cache = map_alloc(sizeof(struct texture *));
                remain_binds = array_alloc(sizeof(i16), UNORDERED);
                /* reserve binding plots */
                i16 i;
                for_i(i, opengl_max_textures) {
                        array_push(remain_binds, &i);
                }
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
        struct texture *t = NULL;

        if(remain_binds->len > 0) {
                bid = array_get(remain_binds, i16, 0);
        } else {
                /* use lowest priority texture in queue */
                t = (struct texture *)(bind_list.prev - offsetof(struct texture, bind_head));
                bid = t->active_id;
        }

        /* allocate texture and data */
        struct texture *r = smalloc(sizeof(struct texture));
        glGenTextures(1, &r->id);
        glActiveTexture(GL_TEXTURE0 + bid);
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
        INIT_LIST_HEAD(&r->bind_head);

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
        struct texture *t = NULL;

        if(remain_binds->len > 0) {
                bid = array_get(remain_binds, i16, 0);
        } else {
                /* use lowest priority texture in queue */
                t = (struct texture *)(bind_list.prev - offsetof(struct texture, bind_head));
                bid = t->active_id;
        }
        /* allocate texture and data */

        struct texture *r = smalloc(sizeof(struct texture));
        glGenTextures(1, &r->id);
        glActiveTexture(GL_TEXTURE0 + bid);
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
        INIT_LIST_HEAD(&r->bind_head);

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
                        array_push(remain_binds, &p->active_id);
                        list_del(&p->bind_head);
                }
                glDeleteTextures(1, &p->id);
                sfree(p);
        }
}

/*
 * bind texture
 */
void texture_bind(struct texture *p)
{
        /* if texture is already binded, set it's priority is highest */
        if(p->active_id >= 0)
        {
                list_del(&p->bind_head);
                list_add(&p->bind_head, &bind_list);
                return;
        }

        /* id to bind */
        i16 bid;

        if(remain_binds->len) {
                /* reserve one remain id */
                bid = array_get(remain_binds, i16, remain_binds->len - 1);
                array_remove(remain_binds, remain_binds->len - 1);
        } else {
                /*
                 * all plots are used, I decide to bind this texture to active_id
                 * of lowest priority texture in queue because there is a chain
                 * of textures needing to bind
                 */
                 struct texture *lowest = (struct texture *)(bind_list.prev - offsetof(struct texture, bind_head));
                 i16 bid = lowest->active_id;
                 lowest->active_id = -1;
                 list_del(&lowest->bind_head);
        }
        /* bind texture to bid */
        p->active_id = bid;
        glActiveTexture(GL_TEXTURE0 + bid);
        glBindTexture(GL_TEXTURE_2D, p->id);
        /* set texture's priority is highest in queue to maintain binding chain */
        list_add(&p->bind_head, &bind_list);
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
