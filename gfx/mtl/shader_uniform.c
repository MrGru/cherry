#include <cherry/graphic/shader.h>

#if GFX == MTL

#include <cherry/memory.h>

/*
 * allocate new shader uniform tracker
 */
struct shader_uniform_track *shader_uniform_track_alloc()
{
        struct shader_uniform_track *p = smalloc(sizeof(struct shader_uniform_track));
        p->uniform      = NULL;
        p->pipeline     = NULL;
        return p;
}

/*
 * deallocate uniform tracker p
 */
void shader_uniform_track_free(struct shader_uniform_track *p)
{
        if(p->uniform) shader_uniform_free(p->uniform);
        sfree(p);
}

#endif
