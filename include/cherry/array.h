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
struct array *array_alloc(uint16_t item_size, uint8_t ordered);

/*
 * deallocate array and content
 */
void array_free(struct array *p);

/*
 * resize array space big enough to hold len item
 * with size defined in array
 */
void array_reserve(struct array *p, uint16_t len);

void array_force_len(struct array *p, uint16_t len);

/*
 * push item_size bytes from d to tail of array ptr
 */
void array_push(struct array *p, void *d);

/*
 * copy item_size bytes from d to index position in array
 */
void array_set(struct array *p, uint16_t index, void *d);

/*
 * get item_size bytes from index position in array
 * and copy to o pinter
 */
void array_copy(struct array *p, uint16_t index, void *o);

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
void array_remove(struct array *p, uint16_t index);

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

#endif