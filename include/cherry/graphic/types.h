/*
 * graphic types definition
 */
#ifndef __CHERRY_GRAPHIC_TYPES_H__
#define __CHERRY_GRAPHIC_TYPES_H__

#include <cherry/types.h>
#include <cherry/graphic/graphic.h>

struct device_buffer;
struct shader;

/*
 * OGL
 * @id          : opengl vbo id
 *
 * MTL
 * @ptr         : bridge pointer to mtl_buffer object
 * @size        : current size of buffer
 */
struct device_buffer {
#if   GFX == OGL
        u32     id;
#elif GFX == MTL
        void    *ptr;
        u64     size;
#endif
};

/*
 * @buffers     : array of device_buffer
 * @pipeline    : current pipeline setting up this group
 *
 * OGL
 * @id          : opengl vao id
 */
struct device_buffer_group {
        struct array    *buffers;
        struct shader   *pipeline;
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
 * mesh vertex type
 * @PS  : position
 * @NR  : normal
 * @TR  : transformation
 * @CL  : color
 */
enum {
        PS_NR_TR_CL,
        PS_NR,
        PS_TR
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
 *
 * OGL
 * name         : attribute name
 *
 * MTL
 * @offset      : attribute offset in buffer
 */
struct shader_attribute_descriptor {
        u8              type;
#if GFX == OGL
        struct string   *name;
#endif
#if GFX == MTL
        u16             offset;
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
};

/*
 * @buffers     : array of buffers used in pipeline
 */
struct shader_descriptor {
        struct array *buffers;
};

/*
 * shader object
 * @tracks      : array of uniform trackers registered
 * @flags       : array of flag to find active trackers
 * @pendings    : array of active trackers indice
 * @update      : flag allows shader to update uniforms datas
 * @mesh_type   : mesh vertex type allowed in pipeline
 *
 * OGL
 * @id          : opengl shader id
 *
 * MTL
 * @uniforms    : mtl uniforms buffers
 * @ptr         : bridge pointer to mtl pipeline object
 */
struct shader {
        struct array                    *tracks;
        struct array                    *flags;
        struct array                    *pendings;
        u8                              update[BUFFERS];
        u8                              mesh_type;
        struct shader_descriptor        *descriptor;
#if GFX == OGL
        u32                             id;
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

#endif
