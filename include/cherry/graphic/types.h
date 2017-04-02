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
        u64     size;
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

struct uniform_type {
        i8      mask;
        u8      type;
        i16     offset;
        i8      frame;
};

struct uniform_buffer {
        void                    *types;
        void                    *data;

        struct array            *update_indice;

        u8                      update;

#if GFX == OGL
        struct shader           *last_pipeline;
#endif

#if GFX == MTL
        struct device_buffer    *buffers[BUFFERS];
#endif
};

struct link_uniform {
        struct array    *ids;
};

/*
 * shader object
 *
 * OGL
 * @id                  : opengl shader id
 *
 * MTL
 * @ptr                 : bridge pointer to mtl pipeline object
 */
struct shader {
#if GFX == OGL
        u32                             id;
        struct array                    *link_uniforms;
#endif
#if GFX == MTL
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
        i32                     active_id;
#endif
#if GFX == MTL
        void*                   ptr;
#endif
};

/*
 * input keys
 */
enum {
        INPUT_POSITION_2D,
        INPUT_POSITION_3D,
        INPUT_NORMAL,
        INPUT_TEXCOORD,
        INPUT_COLOR
};

/*
 * uniform keys
 */
enum {
        UNIFORM_CAMERA,
        UNIFORM_TRANSFORM,
        UNIFORM_LIGHT
};

/*
 * node types
 */
enum {
        NODE_SPRITE
};

/*
 * shader types
 */
enum {
        SHADER_2D_TEXTURE_COLOR,
        SHADER_3D_TEXTURE_COLOR
};

/*
 * camera definition
 */

union camera_uniform_type {
        struct {
                struct uniform_type     project;
                struct uniform_type     view;
                struct uniform_type     position;
        };
        struct uniform_type     types[3];
};

struct camera_uniform {
        union mat4      project;
        union mat4      view;
        union vec3      position;
};

enum {
        CAMERA_UNIFORM_PROJECT,
        CAMERA_UNIFORM_VIEW,
        CAMERA_UNIFORM_POSITION
};

struct camera {
        u8                      position_update;
        struct uniform_buffer   *buffer;
};

/*
 * trasnform definition
 */
union transform_uniform_type {
        struct {
                struct uniform_type     transform;
        };
        struct uniform_type     types[1];
};

struct transform_uniform {
        union mat4      transform;
};

enum {
        TRANSFORM_UNIFORM_TRANSFORM
};

/*
 * light definition
 */
struct direction_light {
        union vec3      direction;
        union vec3      ambient;
        union vec3      diffuse;
        union vec3      specular;
};

struct point_light {
        union vec3      position;
        float           constant;
        float           linear;
        float           quadratic;
        union vec3      ambient;
        union vec3      diffuse;
        union vec3      specular;
};

struct spot_light {
        union vec3      position;
        union vec3      direction;
        float           cutOff;
        float           outerCutOff;

        float           constant;
        float           linear;
        float           quadratic;

        union vec3      ambient;
        union vec3      diffuse;
        union vec3      specular;
};


/*
 * sprite definition
 */
struct sprite_buffer_interface {
        struct device_buffer    *position;
        struct device_buffer    *texcoord;
        struct device_buffer    *color;
};

struct sprite_common_uniform_interface {
        struct uniform_buffer   *camera;
};

struct sprite_uniform_interface {
        struct uniform_buffer  *transform;
};

struct node_manager {
        struct map              *common_uniform_buffers;
        struct map              *shaders;

        struct list_head        transform_queue;
        struct node             *transform_root;
        u8                      transform_full;

        struct list_head        nodes;
};

enum {
        BLEND_NONE,
        BLEND_ALPHA,
        BLEND_MULTIPLY,
        BLEND_ADDITIVE
};

struct node_render_buffer_group {
        struct device_buffer_group      *group;
        u8                              blend_mode;
        int                             vertice_count;
        struct array                    *textures;
};

struct node_render_content {
        struct array    *buffer_groups;
        int             actives;
};

struct node {
        struct list_head                life_head;

        struct list_head                head;
        struct list_head                children;
        struct node                     *parent;

        struct list_head                transform_queue_head;
        struct list_head                updater_head;
        struct list_head                updating_transform_children;
        u8                              update;

        u8                              visible;

        i32                             type;

        union {
                union vec3              position;
                union vec4              position_expand;
        };
        union {
                union vec3              scale;
                union vec4              scale_expand;
        };
        union {
                union vec3              size;
                union vec4              size_expand;
        };
        union {
                union vec3              origin;
                union vec4              origin_expand;
        };
        union vec4                      quaternion;
        union vec4                      quaternion_animation;

        union mat4                      transform;

        void                            *data;

        struct node_manager             *manager;

        struct node_render_content      *current_render_content[BUFFERS];

        struct array                    *current_common_uniform_buffers;

        struct array                    *current_uniform_buffers;

        i32                             shader_type;
        struct shader                   *current_shader;
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
