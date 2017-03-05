/*
 * Copyright (C) 2017 Manh Tran
 *
 * quick approach for allocation and free memory
 * single thread - need implementation for concurrent
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
#include <cherry/memory.h>
#include <cherry/pool.h>
#include <cherry/list.h>
#include <cherry/stdint.h>
#include <cherry/stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_PAGE 4096

#ifndef MEM_DEBUG
        #define MEM_DEBUG 0
#endif

/*
 * store list of pages allocated by malloc, references shared to application
 */
struct mem_track_head {
        struct list_head head;
        int *count;
        void *ptr;
};

/*
 * object head need reference to total objects shared for application
 * in order to dim_memory to work
 */
struct mem_block_head {
        struct pool_head        head;
#if MEM_DEBUG == 1
        u8                      used;
#else
        u8                      padding;
#endif
        struct mem_track_head   *track_head;
};

/*
 * mem_head holds first mem_block_head so we can get information
 * about item_size (e.g need for srealloc to work ).
 *
 * mem_head also keeps track for every memory page allocated by malloc,
 * total objects shared for application in order to dim_memory
 */
struct mem_head {
        struct pool_head head;
        struct mem_track_head track;
        uint32_t item_size;
};

static int table[32] = {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

static struct mem_head blocks[32] = {
        {POOL_HEAD_INIT(blocks[0].head), LIST_HEAD_INIT(blocks[0].track.head), 0},
        {POOL_HEAD_INIT(blocks[1].head), LIST_HEAD_INIT(blocks[1].track.head), 0},
        {POOL_HEAD_INIT(blocks[2].head), LIST_HEAD_INIT(blocks[2].track.head), 0},
        {POOL_HEAD_INIT(blocks[3].head), LIST_HEAD_INIT(blocks[3].track.head), 0},
        {POOL_HEAD_INIT(blocks[4].head), LIST_HEAD_INIT(blocks[4].track.head), 0},
        {POOL_HEAD_INIT(blocks[5].head), LIST_HEAD_INIT(blocks[5].track.head), 0},
        {POOL_HEAD_INIT(blocks[6].head), LIST_HEAD_INIT(blocks[6].track.head), 0},
        {POOL_HEAD_INIT(blocks[7].head), LIST_HEAD_INIT(blocks[7].track.head), 0},
        {POOL_HEAD_INIT(blocks[8].head), LIST_HEAD_INIT(blocks[8].track.head), 0},
        {POOL_HEAD_INIT(blocks[9].head), LIST_HEAD_INIT(blocks[9].track.head), 0},
        {POOL_HEAD_INIT(blocks[10].head), LIST_HEAD_INIT(blocks[10].track.head), 0},
        {POOL_HEAD_INIT(blocks[11].head), LIST_HEAD_INIT(blocks[11].track.head), 0},
        {POOL_HEAD_INIT(blocks[12].head), LIST_HEAD_INIT(blocks[12].track.head), 0},
        {POOL_HEAD_INIT(blocks[13].head), LIST_HEAD_INIT(blocks[13].track.head), 0},
        {POOL_HEAD_INIT(blocks[14].head), LIST_HEAD_INIT(blocks[14].track.head), 0},
        {POOL_HEAD_INIT(blocks[15].head), LIST_HEAD_INIT(blocks[15].track.head), 0},
        {POOL_HEAD_INIT(blocks[16].head), LIST_HEAD_INIT(blocks[16].track.head), 0},
        {POOL_HEAD_INIT(blocks[17].head), LIST_HEAD_INIT(blocks[17].track.head), 0},
        {POOL_HEAD_INIT(blocks[18].head), LIST_HEAD_INIT(blocks[18].track.head), 0},
        {POOL_HEAD_INIT(blocks[19].head), LIST_HEAD_INIT(blocks[19].track.head), 0},
        {POOL_HEAD_INIT(blocks[20].head), LIST_HEAD_INIT(blocks[20].track.head), 0},
        {POOL_HEAD_INIT(blocks[21].head), LIST_HEAD_INIT(blocks[21].track.head), 0},
        {POOL_HEAD_INIT(blocks[22].head), LIST_HEAD_INIT(blocks[22].track.head), 0},
        {POOL_HEAD_INIT(blocks[23].head), LIST_HEAD_INIT(blocks[23].track.head), 0},
        {POOL_HEAD_INIT(blocks[24].head), LIST_HEAD_INIT(blocks[24].track.head), 0},
        {POOL_HEAD_INIT(blocks[25].head), LIST_HEAD_INIT(blocks[25].track.head), 0},
        {POOL_HEAD_INIT(blocks[26].head), LIST_HEAD_INIT(blocks[26].track.head), 0},
        {POOL_HEAD_INIT(blocks[27].head), LIST_HEAD_INIT(blocks[27].track.head), 0},
        {POOL_HEAD_INIT(blocks[28].head), LIST_HEAD_INIT(blocks[28].track.head), 0},
        {POOL_HEAD_INIT(blocks[29].head), LIST_HEAD_INIT(blocks[29].track.head), 0},
        {POOL_HEAD_INIT(blocks[30].head), LIST_HEAD_INIT(blocks[30].track.head), 0},
        {POOL_HEAD_INIT(blocks[31].head), LIST_HEAD_INIT(blocks[31].track.head), 0}
};

/*
 * expand n blocks with size
 */
static inline void __expand(size_t size, int id)
{
        void *p = malloc(MEM_PAGE);

        int n = MEM_PAGE / size;
        int i;
        int *count = malloc(sizeof(int));
        *count = 0;

        /* expand track list */
        struct mem_track_head *track = malloc(sizeof(struct mem_track_head));
        track->ptr = p;
        track->count = count;
        list_add(&track->head, &blocks[id].track.head);

        back_i(i, n) {
                struct mem_block_head *head = (struct mem_block_head *)(p + i * size);
                head->track_head = track;
#if MEM_DEBUG == 1
                head->used = 0;
#endif
                pool_add(&head->head, &blocks[id].head);
        }
}

/*
 * expand single block with size
 */
static inline void __expand_large(size_t size, int id)
{
        /*
         * padding 8 bytes to tail to prevent error out of bound in smemcpy, smemcmp
         */
        void *p = malloc(size);
        /* expand track list */
        struct mem_track_head *track = malloc(sizeof(struct mem_track_head));
        track->ptr = p;
        track->count = malloc(sizeof(int));
        *track->count = 0;
        list_add(&track->head, &blocks[id].track.head);

        struct mem_block_head *head = (struct mem_block_head *)p;
        head->track_head = track;
#if MEM_DEBUG == 1
        head->used = 0;
#endif
        pool_add(&head->head, &blocks[id].head);
}

/*
 * size must be power of two
 */
static inline void *__smalloc(size_t size)
{
        int id = table[(uint32_t)(size * 0x077CB531U) >> 27];

        if(pool_singular(&blocks[id].head)) {
                blocks[id].item_size = size;
                if(size < MEM_PAGE) {
                        __expand(size, id);
                } else {
                        __expand_large(size, id);
                }
        }
        struct mem_block_head * p = (struct mem_block_head *)pool_get(&blocks[id].head);
#if MEM_DEBUG == 1
        p->used = 1;
#endif
        (*p->track_head->count)++;
        return p + 1;
}

/*
 * round size to nearest power of two integer
 */
static inline size_t __sizep2(size_t size)
{
        size += sizeof(struct mem_block_head);
        size--;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size++;
        return size;
}

void *smalloc(size_t size)
{
        return __smalloc(__sizep2(size));
}

/*
 * push back block holding ptr to pool
 */
void sfree(void *ptr)
{
        struct mem_block_head *p = (struct mem_block_head *)ptr - 1;
        (*p->track_head->count)--;
#if MEM_DEBUG == 1
        if(!p->used) {
                debug("error: double free pointer!\n");
        }
        p->used = 0;
#endif
        struct pool_head *b = &p->head;
        pool_add(b, b->pprev);
}

/*
 * return pointer to space big enough to hold size bytes
 */
void *srealloc(void *ptr, size_t size)
{
        size = __sizep2(size);
        if(!ptr) return __smalloc(size);

        struct mem_block_head *p = (struct mem_block_head *)ptr - 1;
        struct pool_head *b = &p->head;
        size_t psize = ((struct mem_head *)(b->pprev))->item_size;

        /* block holding ptr currently has enough space, no need to resize */
        if(psize >= size) return ptr;

        /* allocate new block */
        void *n = __smalloc(size);
        smemcpy(n, ptr, psize);
        sfree(ptr);
        return n;
}

/*
 * fast mem copy
 */
void  smemcpy(void *dst, void *src, volatile size_t len)
{
        /* ignore zero copy */
        if(!len || dst == src) return;

        // /* copy words */
        size_t *dw = dst;
        size_t *sw = src;
        // while(len >= sizeof(size_t)) {
        //         *dw++ = *sw++;
        //         len -= sizeof(size_t);
        // }

        /* copy bytes */
        u8 *d1 = (u8 *)dw;
        u8 *s1 = (u8 *)sw;
        while(len) {
                *d1++ = *s1++;
                len--;
        }
}

int   smemcmp(void *p1, void *p2, volatile size_t len)
{
        if(!len || p1 == p2) return 0;

        size_t *dw = p1;
        size_t *sw = p2;
        while(len >= sizeof(size_t)) {
                if(*dw++ != *sw++) return 1;
                len -= sizeof(size_t);
        }

        char *d1 = (char *)dw;
        char *s1 = (char *)sw;
        while(len) {
                if(*d1++ != *s1++) return 1;
                len--;
        }
        return 0;
}

/*
 * make sure all blocks parted from track will be removed from pool
 */
static inline void test_block(struct mem_head *head, struct mem_track_head *track)
{
        struct pool_head temp = POOL_HEAD_INIT(temp);

        /*
         * move all blocks to temporary pool
         */
        while(!pool_singular(&head->head)) {
                struct mem_block_head * p = (struct mem_block_head *)pool_get(&head->head);
                struct pool_head *b = &p->head;
                /*
                 * set temporary pool
                 */
                b->pprev = &temp;
                pool_add(b, b->pprev);
        }

        /*
         * push blocks not linked to track to real pool again
         */
        while(!pool_singular(&temp)) {
                struct mem_block_head * p = (struct mem_block_head *)pool_get(&temp);
                if(p->track_head != track) {
                        struct pool_head *b = &p->head;
                        /*
                         * set real pool
                         */
                        b->pprev = &head->head;
                        pool_add(b, b->pprev);
                }
        }
}

/*
 * free blocks unused
 */
void dim_memory()
{
	int i;
        int empty = 1;
        for_i(i, 32) {
                struct mem_head *head = &blocks[i];
                struct list_head *lh;
                struct list_head *lhn;
                list_for_each_safe(lh, lhn, &head->track.head) {
                        struct mem_track_head *track = (struct mem_track_head *)lh;
                        if(*track->count == 0) {
                                debug("track %p deallocated\n", track);
                                test_block(head, track);
                                list_del(lh);
                                free(track->ptr);
                                free(track->count);
                                free(track);
                        } else {
                                debug("track %p | item_size %d | using %d\n", track, head->item_size, *track->count);
                                empty = 0;
                        }
                }
        }
        if(empty) {
                debug("all memory blocks are deallocated!\n");
        }
}

/*
 * list of cache functions
 */
struct cache_function {
        struct list_head head;
        void(*f)();
};

static struct list_head cache_head = LIST_HEAD_INIT(cache_head);

/*
 * add function need to be called in cache_free
 */
void cache_add(void(*func)())
{
        struct cache_function *p = malloc(sizeof(struct cache_function));
        list_add_tail(&p->head, &cache_head);
        p->f = func;
}

/*
 * call all functions registered
 * call cache_free one time when application is going to terminate
 */
void cache_free()
{
        if(list_singular(&cache_head)) return;

        struct list_head *p;
        list_while_not_singular(p, &cache_head) {
                struct cache_function *c = (struct cache_function *)p;
                if(c->f) c->f();
                list_del(p);
                free(c);
        }
}
