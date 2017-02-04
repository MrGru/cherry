/*
 * bytes definition
 */
#ifndef __CHERRY_BYTES_H__
#define __CHERRY_BYTES_H__

#include <cherry/types.h>

/*
 * allocate new bytes object
 */
struct bytes *bytes_alloc();

/*
 * deallocate bytes object p and content
 */
void bytes_free(struct bytes *p);

/*
 * resize bytes content big enough to hold size bytes
 */
void bytes_reserve(struct bytes *p, u32 size);

/*
 * append size bytes to p
 */
void bytes_cat(struct bytes *p, void *s, u32 size);

#endif
