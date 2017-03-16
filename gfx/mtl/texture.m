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
#import <cherry/graphic/texture.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/map.h>
#import <cherry/array.h>
#import <cherry/string.h>
#import <cherry/graphic/image.h>

/*
 * texture_house to keep device mtl textures alive
 */
static NSMutableArray *texture_house = nil;

static struct map *texture_cache = NULL;

static void texture_cache_dispose()
{
        map_deep_free(texture_cache, struct texture *, texture_free);
        texture_cache = NULL;
}

static void texture_cache_setup()
{
        if(!texture_cache) {
                texture_cache = map_alloc(sizeof(struct texture *));
                texture_house = [[NSMutableArray alloc] init];
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
        MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat      :image_type(p)
                width                                   :image_width(p)
                height                                  :image_height(p)
                mipmapped                               :NO];
        id<MTLTexture> t = [shared_mtl_device newTextureWithDescriptor:textureDescriptor];

        MTLRegion region = MTLRegionMake2D(0, 0, image_width(p), image_height(p));
        [t replaceRegion:region mipmapLevel:0 withBytes:image_pixels(p) bytesPerRow:p->bytes_per_row];
        [texture_house addObject:t];

        struct texture *r = smalloc(sizeof(struct texture));
        r->width          = image_width(p);
        r->height         = image_height(p);
        r->ptr            = (__bridge void*)(t);
        r->ref            = 0;
        return r;
}

/*
 * allocate new depth texture
 */
struct texture *texture_alloc_depth(u16 width, u16 height)
{
        texture_cache_setup();

        /* create device texture object */
        MTLTextureDescriptor *shadowTextureDesc = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat      : MTLPixelFormatDepth32Float
                width                                   : width
                height                                  : height
                mipmapped                               : NO];
        id<MTLTexture> t =  [shared_mtl_device newTextureWithDescriptor: shadowTextureDesc];

        [texture_house addObject:t];

        /* create application texture object */
        struct texture *ret     = smalloc(sizeof(struct texture));
        ret->width              = width;
        ret->height             = height;
        ret->ptr                = (__bridge void *)(t);
        ret->ref                = 0;
        return ret;
}

/*
 * decrease texture's ref by 1
 * if the texture's ref equal to zero then deallocate it
 */
void texture_free(struct texture *p)
{
        p->ref--;
        if(p->ref <= 0) {
                if(p->ptr) {
                        [texture_house removeObject:(__bridge id _Nonnull)(p->ptr)];
                }
                sfree(p);
        }
}

/*
 * remove unused textures in cache
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
