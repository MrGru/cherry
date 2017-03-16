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
#ifndef __CHERRY_GRAPHIC_TYPES_H__
#define __CHERRY_GRAPHIC_TYPES_H__

#include <cherry/types.h>
#include <cherry/math/types.h>
#include <cherry/graphic/graphic.h>

// #if GFX == OGL

union texcoord_3d {
        struct {
                float x, y, z, w;
        };
        struct {
                float r, g, b, a;
        };
        float v[4];
};

// #elif GFX == MTL
//
// union texcoord_3d {
//         struct {
//                 u32 x, y, z, w;
//         };
//         struct {
//                 u32 r, g, b, a;
//         };
//         u32 v[4];
// };
//
// #endif

struct device_buffer;
struct shader;

enum {
        BUFFER_VERTICE,
        BUFFER_ELEMENT
};

/*
 * @BUFFER_DEVICE : buffer's data is allocated in device/gpu memory
 * @BUFFER_PINNED : buffer's data is allocated in main memory
 */
enum {
        BUFFER_DEVICE,
        BUFFER_PINNED
};

#if GFX == OGL
extern u32 current_bind_buffer;
#endif

/*
 * @ref         : buffer may be shared among objects
 * @type        : buffer type
 *
 * OGL
 * @id          : opengl vbo id
 *
 * MTL
 * @ptr         : bridge pointer to mtl_buffer object
 * @size        : current size of buffer
 */
struct device_buffer {
        i16     ref;
        u8      type;
        u16     item_size;
        u8      location;
#if   GFX == OGL
        u32     id;
#elif GFX == MTL
        void    *ptr;
        u64     size;
#endif
};

/*
 * @buffers     : array of device_buffer
 *
 * OGL
 * @id          : opengl vao id
 */
struct device_buffer_group {
        struct array    *buffers;
#if GFX == OGL
        u32             id;
#endif
};

/*
 * uniform data type
 */
enum {
        UNIFORM_F1,
        UNIFORM_F2,
        UNIFORM_F3,
        UNIFORM_F4,
        UNIFORM_I1,
        UNIFORM_I2,
        UNIFORM_I3,
        UNIFORM_I4,
        UNIFORM_M2,
        UNIFORM_M3,
        UNIFORM_M4
};

/*
 * shader attribute type
 */
enum {
        ATTRIBUTE_FLOAT,
        ATTRIBUTE_VEC2,
        ATTRIBUTE_VEC3,
        ATTRIBUTE_VEC4,
        ATTRIBUTE_MAT2,
        ATTRIBUTE_MAT3,
        ATTRIBUTE_MAT4
};

/*
 * uniform uses this index to keep track to shaders it's linking to
 * @pipeline    : shader that uniform is linking to
 * @index       : index of uniform in shader
 */
struct shader_uniform_index {
        struct shader   *pipeline;
        i16             index;
};

/*
 * uniform object
 * @data        : uniform's data
 * @ref         : life-time counter
 * @indice      : array of shaders that uniform is linking to
 */
struct shader_uniform {
        struct bytes    *data;
        i16             ref;
        struct array    *indice;
};

/*
 * shader uniform tracker
 * @uniform     : link to uniform object
 * @pipeline    : shader owning this tracker
 * @type        : uniform data type
 * @index       : uniform index in shader
 *
 * OGL
 * @name        : opengl uniform name
 * @id          : opengl uniform id
 *
 * MTL
 * @offset      : mtl uniform offset in buffer
 */
struct shader_uniform_track {
        struct shader_uniform   *uniform;
        struct shader           *pipeline;
#if GFX == OGL
        struct string           *name;
        i32                     id;
#endif
        u8                      type;
        i16                     index;
#if GFX == MTL
        i16                     offset;
#endif
};

/*
 * shader attribute descriptor
 * @type        : attribute type
 * @offset      : attribute offset in buffer
 *
 * OGL
 * name         : attribute name
 */
struct shader_attribute_descriptor {
        u8              type;
        u16             offset;
#if GFX == OGL
        struct string   *name;
#endif
};

/*
 * @attributes  : array of attributes used in buffer
 * @vertex_size : one vertex size in buffer
 * @instanced   : indicate buffer is instanced or not
 */
struct shader_buffer_descriptor {
        struct array    *attributes;
        u16             vertex_size;
        u8              instanced;
        u32             divisor;
};

/*
 * @buffers     : array of buffers used in pipeline
 */
struct shader_descriptor {
        struct array *buffers;
};

/*
 * shader object
 * @tracks              : array of uniform trackers registered
 * @flags               : array of flag to find active trackers
 * @pendings            : array of active trackers indice
 * @update              : flag allows shader to update uniforms datas
 * @mesh_types          : mesh vertex types allowed in pipeline
 *                        application should have max 256 mesh types
 *
 * OGL
 * @id                  : opengl shader id
 * @texture_uniforms    : opengl sampler2D ids
 *
 * MTL
 * @uniforms            : mtl uniforms buffers
 * @ptr                 : bridge pointer to mtl pipeline object
 */
struct shader {
        struct array                    *tracks;
        struct array                    *flags;
        struct array                    *pendings;
        u8                              update[BUFFERS];
        struct shader_descriptor        *descriptor;
#if GFX == OGL
        u32                             id;
        struct array                    *texture_uniforms;
#endif
#if GFX == MTL
        struct device_buffer            *uniforms[BUFFERS];
        void                            *ptr;
#endif
};

/*
 * raw image data
 * @ptr                 : image object
 * @type                : image channel type
 *
 * MTL
 * @width               : image width
 * @height              : image height
 * @bytes_per_row       : bytes per pixel * width
 */
struct image {
        void    *ptr;
        i32     type;
#if GFX == MTL
        u16     width;
        u16     height;
        u16     bytes_per_row;
#endif
};

/*
 * device texture
 * @width       : texture width
 * @height      : texture height
 * @ref         : reference counter
 *
 * OGL
 * @id          : opengl texture id
 * @active_id   : current active texture id that texture is binding
 * @bind_head   : node in binding list
 *
 * MTL
 * @ptr         : bridge pointer to mtl texture object
 */
struct texture {
        u16                     width;
        u16                     height;

        i16                     ref;

#if GFX == OGL
        u32                     id;
        i16                     active_id;
        struct list_head        bind_head;
#endif
#if GFX == MTL
        void*                   ptr;
#endif
};

struct texture_frame {
        int             x;
        int             y;
        int             width;
        int             height;
        int             tex_width;
        int             tex_height;
        int             texid;
};

struct camera {
        union mat4              view;
        union vec3              position;
        u8                      position_update;

        struct shader_uniform   *view_uniform;
        struct shader_uniform   *position_uniform;
};

/*
 * render technique
 * I want application to have number draw calls as low as possible
 * read node folder for information
 *
 * each render_content will consume one draw call
 */
struct render_content {
        struct list_head                queue_head;
        struct device_buffer_group      *groups[BUFFERS];
        struct array                    *textures;
        struct map                      *atlases;

        u32                             vertice;
        u32                             max_instances;
        u32                             current_instances;
        u32                             instance_multiple;
        u8                              depth_test;

        struct list_head                node_list;
        struct list_head                pending_updaters;
};

/*
 * each node will have permission to manipulate a small part
 * of render_content's buffers
 */
struct node_data_segment {
        struct list_head        head;
        u8                      frames;
        u32                     start;
        u32                     end;
};

struct node_data {
        u8                              buffer_id;
        struct bytes                    *data;
        struct list_head                segments;
        struct node_data_segment        *fill_segment;
};

struct node {
        struct list_head        content_head;
        u32                     content_index;
        struct render_content   *host;

        struct list_head        updater_head;

        struct list_head        user_head;

        struct array            *pending_datas;
        struct array            *datas;
};

/*
 * render_queue is where to group all render_contents having same pipeline
 * because changing pipeline during rendering is expensive
 */
struct render_queue {
        struct list_head        content_list;
        struct list_head        stage_head;
        struct shader           *pipeline;
};

/*
 * render_stage holds a group of render_queues to render
 * stencil_queue_list renders all it's content to stencil buffer
 */
struct render_stage {
        struct list_head        renderer_head;
        struct list_head        stencil_queue_list;
        struct list_head        content_queue_list;
};

/*
 * render_pass interface for renderer
 */
struct render_pass {
#if     GFX == OGL
        u32     id;
#elif   GFX == MTL
        void    *ptr;
#endif
        void(*del)(struct render_pass *);
};

struct main_render_pass {
        struct render_pass      pass;
};

struct shadow_render_pass {
        struct render_pass      pass;
        struct texture          *map;
};

/*
 * each renderer render to one application pass
 * it is useful when application needs several passes like shadow
 */
struct renderer {
        struct list_head        stage_list;
        struct render_pass      *pass;
        union vec4              *color;
        struct list_head        chain_head;
};

/*
 * event
 */
enum {
        EVENT_KEYBOARD,
        EVENT_MOUSE,
        EVENT_TOUCH
};

enum {
        KEY_DOWN,
        KEY_HOLD,
        KEY_UP
};

enum {
        MOUSE_DOWN,
        MOUSE_MOVE,
        MOUSE_UP,
        MOUSE_CANCEL
};

enum {
        TOUCH_DOWN,
        TOUCH_MOVE,
        TOUCH_UP,
        TOUCH_CANCEL
};

struct event {
        u8      type;
        union {
                /*
                 * keyboard interface
                 */
                struct {
                        int     key;
                        u8      key_state;
                };
                /*
                 * mouse interface
                 */
                struct {
                        int     mouse_x;
                        int     mouse_y;
                        u8      mouse_state;
                };
                /*
                 * touch interface
                 */
                struct {
                        int     touch_x;
                        int     touch_y;
                        u8      touch_state;
                };
        };
};

#endif
