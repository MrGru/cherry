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
#ifndef __CHERRY_TYPES_H__
#define __CHERRY_TYPES_H__

#include <cherry/stdint.h>
#include <cherry/stddef.h>
#include <cherry/stdarg.h>
#include <cherry/stdlib.h>

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

struct utf8_head {
        unsigned int bit_7 : 1;
        unsigned int bit_6 : 1;
        unsigned int bit_5 : 1;
        unsigned int bit_4 : 1;
        unsigned int bit_3 : 1;
        unsigned int bit_2 : 1;
        unsigned int bit_1 : 1;
        unsigned int bit_0 : 1;
} __attribute__((packed));

#endif
