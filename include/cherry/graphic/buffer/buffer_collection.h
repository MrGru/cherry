#ifndef __CHERRY_GRAPHIC_BUFFER_BUFFER_COLLECTION_H__
#define __CHERRY_GRAPHIC_BUFFER_BUFFER_COLLECTION_H__

#include <cherry/graphic/buffer/types.h>

struct device_buffer *buffer_quad_alloc();

struct device_buffer *buffer_vertex_alloc(u16 instances);

struct device_buffer *buffer_texcoord_alloc(u16 instances);

struct device_buffer *buffer_quad_texroot_alloc(u16 instances);

struct device_buffer *buffer_quad_texrange_alloc(u16 instances);

struct device_buffer *buffer_z_alloc(u16 instances);

struct device_buffer *buffer_texid_alloc(u16 instances);

struct device_buffer *buffer_transform_alloc(u16 instances);

struct device_buffer *buffer_color_alloc(u16 instances);

#endif
