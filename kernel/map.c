#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/bytes.h>
#include <cherry/memory.h>

#define MAP_MODULO 10

static inline size_t hash(char* key, size_t key_size)
{
        size_t hash = 5381;
        int i;
        for(i = 0; i < key_size; i++) {
                hash = ((hash << 5) + hash) + key[i];
        }
        size_t ret = hash % MAP_MODULO;
        return ret;
}


struct map *map_alloc(u16 item_size)
{
        int i;
        struct map *p = smalloc(sizeof(struct map));
        p->keys = array_alloc(sizeof(struct array *), ORDERED);
        array_reserve(p->keys, MAP_MODULO);
        for(i = 0; i < MAP_MODULO; ++i) {
                struct array *key = array_alloc(sizeof(struct bytes *), UNORDERED);
                array_push(p->keys, &key);
        }
        p->datas = array_alloc(sizeof(struct array *), ORDERED);
        array_reserve(p->datas, MAP_MODULO);
        for(i = 0; i < MAP_MODULO; ++i) {
                struct array *data = array_alloc(item_size, UNORDERED);
                array_push(p->datas, &data);
        }
        p->total = 0;
        p->item_size = item_size;
        return p;
}

void map_free(struct map *p)
{
        struct array **a;
        struct bytes **data;
        array_for_each(a, p->keys) {
                array_for_each(data, *a) {
                        bytes_free(*data);
                }
                array_free(*a);
        }
        array_free(p->keys);
        array_for_each(a, p->datas) {
                array_free(*a);
        }
        array_free(p->datas);
        sfree(p);
}

void map_set(struct map *p, void *key, size_t key_size, void *d)
{
        size_t index = hash(key, key_size);
        struct array *keys      = array_get(p->keys, struct array *, index);
        struct array *datas     = array_get(p->datas, struct array *, index);
        struct bytes **k;
        size_t i;
        array_for_each_index(k, i, keys) {
                if((*k)->len == key_size && smemcmp(key, (*k)->ptr, key_size) == 0) goto reuse;
        }
new:;
        struct bytes *b = bytes_alloc();
        bytes_cat(b, key, key_size);
        array_push(keys, &b);
        array_push(datas, d);
        p->total++;
        return;
reuse:;
        array_set(datas, i, d);
}

int map_has_key(struct map *p, void *key, size_t key_size)
{
        size_t index = hash(key, key_size);
        struct array *keys      = array_get(p->keys, struct array *, index);
        struct array *datas     = array_get(p->datas, struct array *, index);
        struct bytes **k;
        array_for_each(k, keys) {
                if((*k)->len == key_size && smemcmp(key, (*k)->ptr, key_size) == 0) return 1;
        }
        return 0;
}

void map_remove_key(struct map *p, void *key, size_t key_size)
{
        size_t index = hash(key, key_size);
        struct array *keys      = array_get(p->keys, struct array *, index);
        struct array *datas     = array_get(p->datas, struct array *, index);
        struct bytes **k;
        size_t i;
        array_for_each_index(k, i, keys) {
                if((*k)->len == key_size && smemcmp(key, (*k)->ptr, key_size) == 0) {
                        array_remove(keys, i);
                        bytes_free(*k);
                        array_remove(datas, i);
                        p->total--;
                        return;
                }
        }
}

void map_get_list_data(struct map *p, struct array *list)
{
        list->len = 0;
        if(!p->total) return;

        array_force_len(list, p->total);

        size_t index = 0;
        struct array **datas;
        array_for_each(datas, p->datas) {
                if((*datas)->len) {
                        size_t amount = (*datas)->len * (*datas)->item_size;
                        smemcpy(list->ptr + index, (*datas)->ptr, amount);
                        index += amount;
                }
        }
}

/*
 * @null_pointer : application may crash if use NULL instead of null_pointer
 */
static void** null_pointer = 0;
void *map_get_pointer(struct map *p, void *key, size_t key_size)
{
        size_t index = hash(key, key_size);
        struct array *keys      = array_get(p->keys, struct array *, index);
        struct array *datas     = array_get(p->datas, struct array *, index);
        struct bytes **k;
        size_t i;
        array_for_each_index(k, i, keys) {
                if((*k)->len == key_size && smemcmp(key, (*k)->ptr, key_size) == 0) {
                        return (char*)datas->ptr + i * datas->item_size;
                }
        }
        return &null_pointer;
}
