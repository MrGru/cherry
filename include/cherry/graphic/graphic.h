/*
 * graphic definition
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

                #define glGenVertexArrays glGenVertexArraysOES
                #define glBindVertexArray glBindVertexArrayOES
                #define glDeleteVertexArrays glDeleteVertexArraysOES

                #define glMapBuffer glMapBufferOES
                #define glUnmapBuffer glUnmapBufferOES

                #define glDrawArraysInstanced glDrawArraysInstancedEXT
                #define glVertexAttribDivisor glVertexAttribDivisorEXT
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
