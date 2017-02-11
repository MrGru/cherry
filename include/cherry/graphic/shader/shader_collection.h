#ifndef __CHERRY_GRAPHIC_SHADER_SHADER_COLLECTION_H__
#define __CHERRY_GRAPHIC_SHADER_SHADER_COLLECTION_H__

#include <cherry/graphic/shader/types.h>

/*
 * get shader color from cache
 */
struct shader *shader_color_alloc();

/*
 * get shader texture from cache
 */
struct shader *shader_texture_alloc();

#endif
