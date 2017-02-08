#include <cherry/graphic/render.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/graphic/device_buffer.h>

static inline void begin_stencil()
{
        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT);
        glClearStencil(0);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glColorMask(0, 0, 0, 0);
}

static inline void begin_stencil_content()
{
        glColorMask(255, 255, 255, 255);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);
}

static inline void end_stencil()
{
        glDisable(GL_STENCIL_TEST);
}

static inline void queue_render(struct render_queue *queue, u8 frame)
{
        /* apply pipeline */
        shader_use(queue->pipeline);
        /* bind textures */
        struct list_head *head;
        list_for_each(head, &queue->content_list) {
                struct render_content *content = (struct render_content *)
                        ((void*)head - offsetof(struct render_content, queue_head));
                struct texture **tex;
                i16 i;
                array_for_each_index(tex, i, content->textures) {
                        if(*tex) {
                                texture_bind(*tex);
                                GLint active_id = (GLint)(*tex)->active_id;
                                struct shader_uniform *uniform = array_get(queue->pipeline->texture_uniforms,
                                        struct shader_uniform *, i);
                                shader_uniform_update(uniform, &active_id, sizeof(active_id));
                        }
                }
                /* push uniform datas to device memory */
                shader_update_uniform(queue->pipeline, frame);
                /* bind vao and draw */
                glBindVertexArray(content->groups[frame]->id);
                glDrawArraysInstanced(GL_TRIANGLES, 0, content->mesh->vertice_count, content->mesh->instances);
        }
}

static inline void queue_list_render(struct list_head *list, u8 frame)
{
        struct list_head *head, *next;
        list_for_each_safe(head, next, list) {
                struct render_queue *queue = (struct render_queue *)
                        ((void*)head - offsetof(struct render_queue, stage_head));
                queue_render(queue, frame);
        }
}

static struct render_pass *current_pass = NULL;

void renderer_render(struct renderer *p, u8 frame)
{
        if(current_pass != p->pass && p->pass) {
                glBindFramebuffer(GL_FRAMEBUFFER, p->pass->id);
                current_pass = p->pass;
        }
        if(p->color) glClearColor(devec4(*p->color));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->stage_list) {
                struct render_stage *stage = (struct render_stage *)
                        ((void*)head - offsetof(struct render_stage, renderer_head));
                /* enable and render clipping stencil */
                if(!list_singular(&stage->stencil_queue_list)) {
                        begin_stencil();
                        queue_list_render(&stage->stencil_queue_list, frame);
                        begin_stencil_content();
                }
                /* render main content */
                queue_list_render(&stage->content_queue_list, frame);
                /* disable stencil */
                if(!list_singular(&stage->stencil_queue_list)) {
                        end_stencil();
                }
        }
}

#endif
