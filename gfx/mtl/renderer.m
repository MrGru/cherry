#import <cherry/graphic/render.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/list.h>
#import <cherry/map.h>
#import <cherry/array.h>
#import <cherry/graphic/device_buffer.h>

static inline void begin_stencil()
{

}

static inline void begin_stencil_content()
{

}

static inline void end_stencil()
{

}

static inline void queue_render(struct render_queue *queue, u8 frame)
{
        /* bind textures */
        struct list_head *head;
        list_for_each(head, &queue->content_list) {
                struct render_content *content = (struct render_content *)
                        ((void*)head - offsetof(struct render_content, queue_head));
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

void renderer_render(struct renderer *p, u8 frame)
{
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
