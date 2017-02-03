/*
 * common types definition
 */
#ifndef __CHERRY_TYPES_H__
#define __CHERRY_TYPES_H__

struct list_head {
        struct list_head *prev, *next;
};

struct pool_head {
        struct pool_head *next;
        struct pool_head **pprev;
};

#endif
