#include <cherry/graphic/mesh.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/memory.h>
#include <cherry/map.h>
#include <cherry/array.h>

struct mesh *mesh_alloc()
{
        struct mesh *p = smalloc(sizeof(struct mesh));
        i16 i;
        for_i(i, BUFFERS) {
                p->buffers[i] = map_alloc(sizeof(struct device_buffer *));
        }
        return p;
}

void mesh_add(struct mesh *p, struct device_buffer *b[BUFFERS], u8 bid)
{
        i16 i;
        for_i(i, BUFFERS) {
                b[i]->ref++;
                map_set(p->buffers[i], qpkey(bid), &b[i]);
        }
}

void mesh_free(struct mesh *p)
{
        i16 i;
        for_i(i, BUFFERS) {
                map_deep_free(p->buffers[i], struct device_buffer *, device_buffer_free);
        }
        sfree(p);
}
