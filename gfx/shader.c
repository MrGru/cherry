#include <cherry/graphic/shader.h>
#include <cherry/array.h>

/*
 * @p           : pipeline to manipulate
 * @index       : uniform track index
 * @u           : uniform set to index position
 */
void shader_set_uniform(struct shader *p, i16 index, struct shader_uniform *u)
{
        if(index < p->tracks->len) {
                shader_uniform_track_set(array_get(p->tracks, struct shader_uniform_track *, index), u);
        }
}
