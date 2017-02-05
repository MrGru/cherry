/*
 * image definition
 */
#ifndef __CHERRY_GRAPHIC_IMAGE_H__
#define __CHERRY_GRAPHIC_IMAGE_H__

#include <cherry/graphic/types.h>

/*
 * allocate new image object from file path
 */
struct image *image_alloc(char *file);

/*
 * deallocate image object
 */
void image_free(struct image *p);

/*
 * get image width
 */
u16 image_width(struct image *p);

/*
 * get image height
 */
u16 image_height(struct image *p);

/*
 * get image pixels
 */
void *image_pixels(struct image *p);

/*
 * get image type
 */
i32 image_type(struct image *p);

#endif
