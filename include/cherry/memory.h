/*
 * memory definition
 */
#ifndef __CHERRY_MEMORY_H__
#define __CHERRY_MEMORY_H__

#include <stddef.h>

void *smalloc(size_t size);
void *srealloc(void *ptr, size_t size);
void  sfree(void *ptr);
void  smemcpy(void *dst, void *src, size_t length);
void  dim_memory();

#endif
