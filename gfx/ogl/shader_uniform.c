#include <cherry/graphic/shader.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/stdio.h>

/*
 * allocate new shader uniform tracker
 */
struct shader_uniform_track *shader_uniform_track_alloc()
{
        struct shader_uniform_track *p = smalloc(sizeof(struct shader_uniform_track));
        p->uniform      = NULL;
        p->pipeline     = NULL;
        p->name         = string_alloc(0);
        p->id           = -1;
        return p;
}

/*
 * deallocate uniform tracker p
 */
void shader_uniform_track_free(struct shader_uniform_track *p)
{
        if(p->uniform) shader_uniform_free(p->uniform);
        string_free(p->name);
        sfree(p);
}

#endif
