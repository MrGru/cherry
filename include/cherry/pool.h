/*
 * pool implementation
 */
#ifndef __CHERRY_POOL_H__
#define __CHERRY_POOL_H__

#include <cherry/types.h>

#define POOL_HEAD_INIT(p) {&(p), &(p)}

static inline void INIT_POOL_HEAD(struct pool_head *head)
{
        head->next = head;
        head->pprev = head;
}

static inline void pool_add(struct pool_head *new, struct pool_head *head)
{
        struct pool_head *first = head->pprev;
        new->next = first->next;
        first->next = new;
        new->pprev = head->pprev;
}

static inline struct pool_head *pool_get(struct pool_head *head)
{
        struct pool_head *first = head->pprev;
        struct pool_head *next = first->next;
        first->next = next->next;
        return next;
}

static inline int pool_singular(struct pool_head *head)
{
        struct pool_head *first = head->pprev;
        return first == first->next;
}

#endif
