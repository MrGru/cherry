#import <cherry/graphic/render.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/graphic/texture.h>
#import <cherry/stdio.h>

struct render_pass *render_pass_main_alloc()
{
        return NULL;
}

struct render_pass *render_pass_shadow_alloc()
{
        return NULL;
}
#endif
