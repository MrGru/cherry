/*
 * shader definition
 */
#ifndef __CHERRY_GRAPHIC_SHADER_H__
#define __CHERRY_GRAPHIC_SHADER_H__

#include <cherry/graphic/types.h>

/*
 * allocate new shader uniform
 */
struct shader_uniform *shader_uniform_alloc();

/*
 * @p   : shader uniform to free
 */
void shader_uniform_free(struct shader_uniform *p);

/*
 * @p           : shader uniform to update
 * @bytes       : bytes array
 * @len         : bytes array len
 */
void shader_uniform_update(struct shader_uniform *p, void *bytes, u32 len);

/*
 * submit data of shader uniform p
 */
void shader_uniform_submit(struct shader_uniform *p);

/*
 * allocate new shader uniform tracker
 */
struct shader_uniform_track *shader_uniform_track_alloc();

/*
 * link uniform u to tracker p
 */
void shader_uniform_track_set(struct shader_uniform_track *p, struct shader_uniform *u);

/*
 * deallocate uniform tracker p
 */
void shader_uniform_track_free(struct shader_uniform_track *p);

/*
 * allocate new shader attribute descriptor
 */
#if     GFX == OGL
struct shader_attribute_descriptor *__shader_attribute_descriptor_alloc(u8 type, u16 offset, char *name);
#define shader_attribute_descriptor_alloc(type, offset, name) __shader_attribute_descriptor_alloc(type, offset, name)
#elif   GFX == MTL
struct shader_attribute_descriptor *__shader_attribute_descriptor_alloc(u8 type, u16 offset);
#define shader_attribute_descriptor_alloc(type, offset, name) __shader_attribute_descriptor_alloc(type, offset)
#endif

/*
 * deallocate shader attribute descriptor
 */
void shader_attribute_descriptor_free(struct shader_attribute_descriptor *p);

/*
 * allocate new shader buffer descriptor
 */
struct shader_buffer_descriptor *shader_buffer_descriptor_alloc(u16 vertex_size, u8 instanced);

/*
 * deallocate shader buffer descriptor
 */
void shader_buffer_descriptor_free(struct shader_buffer_descriptor *p);

/*
 * allocate new shader descriptor
 */
struct shader_descriptor *shader_descriptor_alloc();

/*
 * deallocate shader descriptor
 */
void shader_descriptor_free(struct shader_descriptor *p);

/*
 * allocate new shader object
 */
struct shader *shader_alloc(char *vert, char *frag, struct shader_descriptor *des);

/*
 * deallocate shader object
 */
void shader_free(struct shader *p);

/*
 * reserve uniform
 * @index       : uniform global index
 * @type        : uniform type
 * @name        : uniform name
 * @offset      : uniform offset
 */
#if   GFX == OGL
void __shader_reserve_uniform(struct shader *p, i16 index, u8 type, char *name);
#define shader_reserve_uniform(p, i, t, n, o) __shader_reserve_uniform(p, i, t, n)
#elif GFX == MTL
void __shader_reserve_uniform(struct shader *p, i16 index, u8 type, i16 offset);
#define shader_reserve_uniform(p, i, t, n, o) __shader_reserve_uniform(p, i, t, o)
#endif

/*
 * @p           : pipeline to manipulate
 * @index       : uniform track index
 * @u           : uniform set to index position
 */
void shader_set_uniform(struct shader *p, i16 index, struct shader_uniform *u);

/*
 * set current active pipeline
 */
#if   GFX == OGL
void shader_use(struct shader *p);

void shader_setup_group(struct shader *p, struct device_buffer_group *g);
#endif

/*
 * update uniform datas
 * @p           : pipeline to manipulate
 * @frame       : current application frame
 */
void shader_update_uniform(struct shader *p, u8 frame);

#endif
