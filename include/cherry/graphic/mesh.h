#ifndef __CHERRY_GRAPHIC_MESH_H__
#define __CHERRY_GRAPHIC_MESH_H__

#include <cherry/graphic/types.h>

struct mesh *mesh_alloc();

void mesh_add(struct mesh *p, struct device_buffer *b[BUFFERS], u8 bid);

void mesh_free(struct mesh *p);

#endif
