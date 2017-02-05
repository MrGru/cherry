/*
 * memory definition
 */
#ifndef __CHERRY_MEMORY_H__
#define __CHERRY_MEMORY_H__

#include <stddef.h>

/*
 * allocate new block from memory pool
 * @size        : block size
 */
void *smalloc(size_t size);

/*
 * if block holding ptr has size smaller than 'size'
 * then allocate new block
 * else reuse it
 */
void *srealloc(void *ptr, size_t size);

/*
 * push block holding ptr to memory pool
 */
void  sfree(void *ptr);

/*
 * copy length bytes from src to dst
 */
void  smemcpy(void *dst, void *src, size_t length);

/*
 * compare length bytes of p2 and p1
 * return 0 if equal
 * return 1 if different
 */
int   smemcmp(void *p1, void *p2, size_t length);

/*
 * remove all blocks unused in memory pool
 */
void  dim_memory();

/*
 * add function need to be called in cache_free
 */
void cache_add(void(*func)());

/*
 * call all functions registered
 * call cache_free one time when application is going to terminate
 * all cache functions will be removed after calling cache_free
 */
void cache_free();

#endif
