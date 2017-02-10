/*
 * dynamic array definition
 */
#ifndef __CHERRY_ARRAY_H__
#define __CHERRY_ARRAY_H__

#include <cherry/types.h>

/*
 * allocate new array
 * @item_size : size of object stored in array
 * @ordered   : non zero for unordered list and else
 */
struct array *array_alloc(u16 item_size, u8 ordered);

/*
 * deallocate array and content
 */
void array_free(struct array *p);

/*
 * resize array space big enough to hold len item
 * with size defined in array
 */
void array_reserve(struct array *p, u16 len);

/*
 * after this call p's len is greater than or equal to len
 */
void array_force_len(struct array *p, u16 len);

/*
 * push item_size bytes from d to tail of array ptr
 */
void array_push(struct array *p, void *d);

/*
 * copy item_size bytes from d to index position in array
 */
void array_set(struct array *p, u16 index, void *d);

/*
 * get item_size bytes from index position in array
 * and copy to o pinter
 */
void array_copy(struct array *p, u16 index, void *o);

/*
 * fast set zero all bytes in array
 */
void array_zero(struct array *p);

/*
 * resize array by remove bytes in index position
 *
 * array will perform translate all bytes from
 * index+1 position to the left if ordered
 *
 * array will swap last item to index position if unordered
 */
void array_remove(struct array *p, u16 index);

/*
 * loop through array by pointer
 */
#define array_for_each(p, a) \
        for(p = (a)->ptr; (size_t)p < (a)->end; p++)

#define array_for_each_index(p, i, a) \
        for(p = (a)->ptr, i = 0; (size_t)p < (a)->end; p++, i++)

/*
 * loop through array by index
 */
#define array_for_index(i, a) \
        for(i = 0; i < a->len; i++)

/*
 * get element at index i of array a and cast to type t
 */
#define array_get(a, t, i) (((t*)(a)->ptr)[i])

/*
 * deallocate array and content
 */
#define array_deep_free(a, type, free_func)     \
        {					\
                type *__p;                      \
                array_for_each(__p, (a)) {      \
                        free_func(*__p);        \
                }                               \
                array_free(a);                  \
        }

#define array_deep_free_safe(a, type, free_func)        \
        {					        \
                type *__p;                              \
                array_for_each(__p, (a)) {              \
                        if(*__p)                        \
                                free_func(*__p);        \
                }                                       \
                array_free(a);                          \
        }

#endif
