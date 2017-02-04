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
        #elif OS == MAC
                #define __gl_h_
                #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

                #include <OpenGL/gl3.h>
                #include <OpenGL/glu.h>
                #include <OpenGL/glext.h>
                #include <GLUT/glut.h>
        #else
                #define GL_GLEXT_PROTOTYPES
                #include <GL/glut.h>
                #include <GL/gl.h>
                #include <GL/glext.h>
        #endif

#elif GFX == MTL
        #import <Metal/Metal.h>

        /* whole game uses single mtl device */
        extern id<MTLDevice> shared_mtl_device;
        /* shader generator */
        extern id<MTLLibrary> shared_mtl_library;

        extern id<MTLCommandBuffer> shared_mtl_command_buffer;

        extern MTLRenderPassDescriptor* shared_mtl_main_pass;

        extern MTLPixelFormat shared_mtl_pixel_format;

        extern NSUInteger shared_mtl_sample_count;
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
