#include <cherry/graphic/graphic.h>

#if GFX == OGL

/*
 * most opengles device support 8 active textures
 *
 * application should check for max textures at runtime
 * and change this value
 */
int opengl_max_textures         = 8;

/*
 * most versions of opengl from 2.1 and opengles from 2.0
 * support depth texture
 *
 * application should check for depth texture extension at runtime
 * and change this value
 */
int depth_texture_enable        = 1;

#endif
