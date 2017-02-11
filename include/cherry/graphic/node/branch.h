#ifndef __CHERRY_GRAPHIC_NODE_BRANCH_H__
#define __CHERRY_GRAPHIC_NODE_BRANCH_H__

#include <cherry/graphic/node/types.h>

/*
 * branch z definitions
 */
struct branch_z *branch_z_alloc(u8 bid);

void branch_z_free(struct branch_z *p);

void branch_z_add(struct branch_z *parent, struct branch_z *child);

void branch_z_del(struct branch_z *p);

void branch_z_traverse(struct branch_z *p, float *cz);

/*
 * branch transform definitions
 */
struct branch_transform *branch_transform_alloc(u8 bid);

void branch_transform_free(struct branch_transform *p);

void branch_transform_add(struct branch_transform *parent, struct branch_transform *child);

void branch_transform_del(struct branch_transform *p);

void branch_transform_traverse(struct branch_transform *p, union mat4 cm);

#endif
