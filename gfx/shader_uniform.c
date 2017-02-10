#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/bytes.h>
#include <cherry/string.h>
#include <cherry/graphic/shader.h>

/*
 * allocate new shader uniform
 */
struct shader_uniform *shader_uniform_alloc()
{
        struct shader_uniform *p = smalloc(sizeof(struct shader_uniform));
        p->data = bytes_alloc();
        p->ref  = 0;
        p->indice = array_alloc(sizeof(struct shader_uniform_index *), UNORDERED);
        return p;
}

/*
 * @p   : shader uniform to free
 */
void shader_uniform_free(struct shader_uniform *p)
{
        p->ref--;
        if(p->ref <= 0) {
                bytes_free(p->data);
                array_deep_free(p->indice, struct shader_uniform_index *, sfree);
                sfree(p);
        }
}

/*
 * @p           : shader uniform to update
 * @bytes       : bytes array
 * @len         : bytes array len
 */
void shader_uniform_update(struct shader_uniform *p, void *bytes, u32 len)
{
        /* update data */
        p->data->len = 0;
        bytes_cat(p->data, bytes, len);

        /* submit uniform to pipeline */
        shader_uniform_submit(p);
}

/*
 * submit data of shader uniform p
 */
void shader_uniform_submit(struct shader_uniform *p)
{
        struct shader_uniform_index **index;
        array_for_each(index, p->indice) {
                struct shader *pipeline = (*index)->pipeline;
                i16 idx                 = (*index)->index;
                i16 num                 = array_get(pipeline->flags, i16, idx);
                if(num < BUFFERS) {
                        /* activate uniform in pipeline */
                        if(num == 0) {
                                array_push(pipeline->pendings, &idx);
                        }
                        /* reset uniform flag counter */
                        num = BUFFERS;
                        array_set(pipeline->flags, idx, &num);
                        /* activate upload permission */
                        int i;
                        for_i(i, BUFFERS) pipeline->update[i] = 1;
                }
        }
}


/*
 * link uniform u to tracker p
 */
void shader_uniform_track_set(struct shader_uniform_track *p, struct shader_uniform *u)
{
        /* ignore relink uniform to tracker */
        if(p->uniform == u) return;

        /* detach previous linked uniform */
        if(p->uniform) {
                struct shader_uniform_index **index;
                u16 i;
                array_for_each_index(index, i, p->uniform->indice) {
                        if((*index)->pipeline == p->pipeline) {
                                sfree(*index);
                                array_remove(p->uniform->indice, i);
                                break;
                        }
                }
                shader_uniform_free(p->uniform);
        }

        /* attach new uniform */
        p->uniform = u;
        if(p->uniform) {
                p->uniform->ref++;
                struct shader_uniform_index *index = smalloc(sizeof(struct shader_uniform_index));
                index->pipeline = p->pipeline;
                index->index = p->index;
                array_push(p->uniform->indice, &index);
                shader_uniform_submit(p->uniform);
        }
}
