/*
 * map definition
 */
#ifndef __CHERRY_MAP_H__
#define __CHERRY_MAP_H__

#include <cherry/types.h>

/*
 * allocate new map
 */
struct map *map_alloc(u16 item_size);

/*
 * deallocate map and content
 */
void map_free(struct map *p);

void map_set(struct map *p, void *key, size_t key_size, void *d);

int map_has_key(struct map *p, void *key, size_t key_size);

void map_remove_key(struct map *p, void *key, size_t key_size);

void map_get_list_data(struct map *p, struct array *list);

void *map_get_pointer(struct map *p, void *key, size_t key_size);

#define map_get(m, t, key...) (*(t*)map_get_pointer(m, key))

/*
 * quick keys
 */
#define qlkey(s) s,sizeof(s)-1

#define qckey(s) s,strlen(s)

#define qpkey(p) &p,sizeof(p)

#define qskey(s) s->str,s->len

#define map_deep_free(map, item_type, free_func)                                \
        {                                                                       \
                struct array *__items = array_alloc(sizeof(item_type), ORDERED);\
                map_get_list_data(map, __items);                                \
                item_type *__item;                                              \
                array_for_each(__item, __items) {                               \
                        free_func(*__item);                                     \
                }                                                               \
                array_free(__items);                                            \
                map_free(map);                                                  \
        }

#endif
