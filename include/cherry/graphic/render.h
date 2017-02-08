#ifndef __CHERRY_GRAPHIC_RENDER_H__
#define __CHERRY_GRAPHIC_RENDER_H__

#include <cherry/graphic/types.h>

/*
 * allocate new render_queue
 * @stage       : render_stage that queue attachs to
 * @pipeline    : program used to render content list
 */
struct render_queue *render_queue_alloc(struct list_head *stage, struct shader *pipeline);

void render_queue_free(struct render_queue *queue);

/*
 * allocate new render_content
 * content groups are built from queue's pipeline and mesh's buffers
 */
struct render_content *render_content_alloc(struct render_queue *queue, struct mesh *mesh);

void render_content_set_texture(struct render_content *content, u16 index, struct texture *t);

void render_content_free(struct render_content *content);

/*
 * allocate new render_stage
 * @renderer    : renderer that stage attachs to
 */
struct render_stage *render_stage_alloc(struct renderer *renderer);

void render_stage_free(struct render_stage *p);

/*
 * allocate new renderer
 */
struct renderer *renderer_alloc();
void renderer_render(struct renderer *p);
void renderer_free(struct renderer *p);

#endif
