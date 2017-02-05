/*
 * texture definition
 */
#ifndef __CHERRY_GRAPHIC_TEXTURE_H__
#define __CHERRY_GRAPHIC_TEXTURE_H__

#include <cherry/graphic/types.h>

/*
 * get texture from cache by file
 */
struct texture *texture_alloc_file(char *file);

/*
 * allocate new texture from image
 */
struct texture *texture_alloc_image(struct image *p);

/*
 * allocate new depth texture
 */
struct texture *texture_alloc_depth(u16 width, u16 height);

/*
 * decrease texture's ref by 1
 * if the texture's ref equal to zero then deallocate it
 */
void texture_free(struct texture *p);

void texture_remove_unused();

#endif
