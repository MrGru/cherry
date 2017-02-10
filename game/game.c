#include <cherry/game/game.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/render.h>
#include <cherry/stdio.h>
#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/shader.h>
#include <cherry/math/math.h>

struct game *game_alloc()
{
        struct game *p  = smalloc(sizeof(struct game));
        p->renderers    = array_alloc(sizeof(struct renderer *), ORDERED);
        p->frame        = 0;

        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        array_push(p->renderers, &r);

        struct render_stage *stage = render_stage_alloc(r);

        struct shader *s = shader_color_alloc();

        struct render_queue *queue = render_queue_alloc(&stage->content_queue_list, s);

        struct array *buffers[BUFFERS];
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                struct device_buffer *quad      = buffer_quad_alloc();
                struct device_buffer *z         = buffer_z_alloc(900);
                struct device_buffer *transform = buffer_transform_alloc(900);
                struct device_buffer *color     = buffer_color_alloc(900);
                array_push(buffers[i], &quad);
                array_push(buffers[i], &z);
                array_push(buffers[i], &transform);
                array_push(buffers[i], &color);
        }

        struct render_content *content = render_content_alloc(queue, buffers, 6, 900);

        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        struct shader_uniform *project_uniform  = shader_uniform_alloc();
        struct shader_uniform *view_uniform     = shader_uniform_alloc();
        shader_set_uniform(s, SHADER_COLOR_PROJECT, project_uniform);
        shader_set_uniform(s, SHADER_COLOR_VIEW, view_uniform);

        //
        // shader_uniform_update(project_uniform, mat4_identity.m, sizeof(mat4_identity));
        // shader_uniform_update(view_uniform, mat4_identity.m, sizeof(mat4_identity));

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
