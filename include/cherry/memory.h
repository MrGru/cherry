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
#ifndef __CHERRY_MEMORY_H__
#define __CHERRY_MEMORY_H__

#include <stddef.h>
#include <string.h>

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
void  smemcpy(void *dst, void *src, volatile size_t length);

/*
 * compare length bytes of p2 and p1
 * return 0 if equal
 * return 1 if different
 */
int   smemcmp(void *p1, void *p2, size_t length);

static inline void smemset(void *p, int val, size_t length)
{
        memset(p, val, length);
}

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
