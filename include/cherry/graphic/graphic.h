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
#ifndef __CHERRY_GRAPHIC_H__
#define __CHERRY_GRAPHIC_H__

#include <cherry/platform.h>
#include <cherry/stdint.h>

/*
 * define application graphics protocol
 */
#define OGL 0
#define MTL 1

#ifndef GFX
#define GFX OGL
#endif

/*
 * define application image protocol
 * use SDL2 as default
 */
#define IMAGE_SDL2 1
#define IMAGE_OBJC 2

#ifndef IMAGE_TYPE
#define IMAGE_TYPE IMAGE_SDL2
#endif

/*
 * define double, triple ... bufferings
 */
#define BUFFERS 3

#if GFX == OGL
        extern int opengl_max_textures;
        extern int depth_texture_enable;
        #if OS == IOS
                #define GL_GLEXT_PROTOTYPES
                #include <OpenGLES/ES2/gl.h>
                #include <OpenGLES/ES2/glext.h>

                #define glGenVertexArrays               glGenVertexArraysOES
                #define glBindVertexArray               glBindVertexArrayOES
                #define glDeleteVertexArrays            glDeleteVertexArraysOES

                #define glMapBuffer                     glMapBufferOES
                #define glUnmapBuffer                   glUnmapBufferOES
                #define glMapBufferRange                glMapBufferRangeEXT

                #define GL_MAP_WRITE_BIT                GL_MAP_WRITE_BIT_EXT
                #define GL_MAP_READ_BIT                 GL_MAP_READ_BIT_EXT
                #define GL_MAP_INVALIDATE_RANGE_BIT     GL_MAP_INVALIDATE_RANGE_BIT_EXT
                #define GL_MAP_INVALIDATE_BUFFER_BIT    GL_MAP_INVALIDATE_BUFFER_BIT_EXT
                #define GL_MAP_FLUSH_EXPLICIT_BIT       GL_MAP_FLUSH_EXPLICIT_BIT_EXT
                #define GL_MAP_UNSYNCHRONIZED_BIT       GL_MAP_UNSYNCHRONIZED_BIT_EXT

                #define glDrawArraysInstanced           glDrawArraysInstancedEXT
                #define glVertexAttribDivisor           glVertexAttribDivisorEXT
        #elif OS == OSX
                #define __gl_h_
                #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

                #include <OpenGL/gl3.h>
                #include <OpenGL/glu.h>
                #include <OpenGL/glext.h>
                #include <GLUT/glut.h>
        #elif OS == DROID
                #define GL_GLEXT_PROTOTYPES
                #include <GLES2/gl2.h>
                #include <GLES2/gl2ext.h>

                #define glGenVertexArrays glGenVertexArraysOES
                #define glBindVertexArray glBindVertexArrayOES
                #define glDeleteVertexArrays glDeleteVertexArraysOES

                #define glMapBuffer glMapBufferOES
                #define glUnmapBuffer glUnmapBufferOES

                #define glDrawArraysInstanced glDrawArraysInstancedEXT
                #define glVertexAttribDivisor glVertexAttribDivisorEXT
        #else
                #include <GL/glut.h>
                #include <GLES3/gl3.h>
        #endif

#elif GFX == MTL

#endif

float pack_rgb_to_float(int r, int g, int b);

/*
 * screen width and height, application needs change these values
 */
extern int video_width;
extern int video_height;

/*
 * shadow size to init depth texture, depth framebuffer ...
 */
extern int shadow_width;

#endif
