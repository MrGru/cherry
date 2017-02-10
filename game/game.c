#include <cherry/game/game.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/render.h>
#include <cherry/stdio.h>
#include <cherry/graphic/shader/shader_collection.h>

struct game *game_alloc()
{
        struct game *p  = smalloc(sizeof(struct game));
        p->renderers    = array_alloc(sizeof(struct renderer *), ORDERED);
        p->frame        = 0;

        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        array_push(p->renderers, &r);

        struct shader *s = shader_color_alloc();

        return p;
}

void game_update(struct game *p)
{

}

void game_render(struct game *p)
{
        /* process all renderers */
        struct renderer **r;
        array_for_each(r, p->renderers) {
                renderer_render(*r, p->frame);
        }
        /* increase frame by 1 */
        p->frame++;
        p->frame %= BUFFERS;
}

void game_free(struct game *p)
{
        array_deep_free(p->renderers, struct renderer *, renderer_free);
        sfree(p);
}
