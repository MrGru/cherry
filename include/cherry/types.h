/*
 * common types definition
 */
#ifndef __CHERRY_TYPES_H__
#define __CHERRY_TYPES_H__

#include <cherry/stdint.h>
#include <cherry/stddef.h>

/*
 * @prev : point to previous head
 * @next : point to next head
 */
struct list_head {
        struct list_head *prev, *next;
};

/*
 * @next        : point to next head
 * @pprev       : point to first head of list
 */
struct pool_head {
        struct pool_head *next;
        struct pool_head *pprev;
};

enum {
        UNORDERED       = 0,
        ORDERED         = 1
};

/*
 * @len         : total objects stored in array
 * @item_size   : size of object
 * @ordered     : see array_remove document at <cherry/array.h>
 * @end         : address at the end of array
 * @ptr         : pointer to array's content array
 */
struct array {
        u16     len;
        u16     item_size;
        u8      ordered;
        size_t  end; /* address array out of bound */
        void    *ptr;
};

/*
 * @item_size   : size of object
 * @total       : total objects stored in map
 * @keys        : array to get key
 * @datas       : array to get object
 */
struct map {
        u16             item_size;
        u16             total;
        struct array    *keys;
        struct array    *datas;
};

/*
 * @len : length of char array
 * @ptr : pointer to char array
 */
struct string {
        u32             len;
        char            *ptr;
};

/*
 * @len : len of byte array
 * @ptr : pointer to byte array
 */
struct bytes {
        u32             len;
        char            *ptr;
};

#endif
