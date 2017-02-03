/*
 * common types definition
 */
#ifndef __CHERRY_TYPES_H__
#define __CHERRY_TYPES_H__

#include <stdint.h>
#include <stddef.h>

struct list_head {
        struct list_head *prev, *next;
};

struct pool_head {
        struct pool_head *next;
        struct pool_head **pprev;
};

enum {
        UNORDERED       = 0,
        ORDERED         = 1
};

struct array {
        uint16_t        len;
        uint16_t        item_size;
        uint8_t         ordered;
        size_t          end; /* address array out of bound */
        void            *ptr;
};

struct map {
        uint16_t        item_size;
        uint16_t        total;
        struct array    *keys;
        struct array    *datas;
};

struct string {
        uint32_t        len;
        char            *ptr;
};

struct bytes {
        uint32_t        len;
        char            *ptr;
};

#endif
