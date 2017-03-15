/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#import <cherry/graphic/render.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/list.h>
#import <cherry/map.h>
#import <cherry/array.h>
#import <cherry/graphic/shader.h>
#import <cherry/graphic/device_buffer.h>
#import <cherry/graphic/texture.h>

static id<MTLRenderCommandEncoder> encoder      = nil;
static id<MTLDepthStencilState> depthless       = nil;
static id<MTLDepthStencilState> depthnone       = nil;
static id<MTLSamplerState>   sampler         = nil;
static u8 depth_testing = 0;

static void free_depth()
{
        if(depthless) {
                depthless = nil;
                depthnone = nil;
                sampler   = nil;
        }
}

static void setup()
{
        if(!depthless) {
                cache_add(free_depth);
                {
                        MTLDepthStencilDescriptor * ds = [MTLDepthStencilDescriptor new];
                        ds.depthWriteEnabled = YES;
                        ds.depthCompareFunction = MTLCompareFunctionLess;
                        depthless = [shared_mtl_device newDepthStencilStateWithDescriptor:ds];
                }

                {
                        MTLDepthStencilDescriptor * ds = [MTLDepthStencilDescriptor new];
                        ds.depthWriteEnabled = NO;
                        ds.depthCompareFunction = MTLCompareFunctionAlways;
                        depthnone = [shared_mtl_device newDepthStencilStateWithDescriptor:ds];
                }
                {
                        MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
                        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
                        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
                        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

                        sampler = [shared_mtl_device newSamplerStateWithDescriptor:samplerDescriptor];
                }
        }
}

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
        /* draw */
        [encoder setRenderPipelineState:(__bridge id _Nonnull)(queue->pipeline->ptr)];

        struct list_head *head;
        list_for_each(head, &queue->content_list) {
                struct render_content *content = (struct render_content *)
                        ((void*)head - offsetof(struct render_content, queue_head));
                /* push uniform datas to device memory */
                shader_update_uniform(queue->pipeline, frame);
                /* update buffers */
                struct list_head *updater, *next_updater;
                list_for_each_safe(updater, next_updater, &content->pending_updaters) {
                        struct node *node = (struct node *)
                                ((void*)updater - offsetof(struct node, updater_head));
                        struct node_data **data;
                        i16 data_index;
                        array_for_each_index(data, data_index, node->pending_datas) {
                                struct device_buffer *buffer = array_get(content->groups[frame]->buffers,
                                        struct device_buffer *, (*data)->buffer_id);
                                struct list_head *segment_head, *segment_head_next;
                                list_for_each_safe(segment_head, segment_head_next, &(*data)->segments) {
                                        struct node_data_segment *seg = (struct node_data_segment *)
                                                ((void *)segment_head - offsetof(struct node_data_segment, head));
                                        seg->frames--;
                                        device_buffer_sub(buffer, node->content_index * (*data)->data->len + seg->start,
                                                (*data)->data->ptr + seg->start, seg->end - seg->start + 1);
                                        if(seg->frames == 0) {
                                                list_del_init(segment_head);
                                        }
                                }
                                if(list_singular(&(*data)->segments)) {
                                        array_remove(node->pending_datas, data_index);
                                        data_index--;
                                        data--;
                                }
                        }
                        if(node->pending_datas->len == 0) list_del_init(updater);
                }
                /* bind vao and draw */
                if(content->current_instances) {
                        if(content->depth_test) {
                                // if(!depth_testing) {
                                        [encoder setDepthStencilState:depthless];
                                        // depth_testing = 1;
                                // }
                        } else {
                                // if(depth_testing) {
                                        [encoder setDepthStencilState:depthnone];
                                        // depth_testing = 0;
                                // }
                        }
                        struct device_buffer_group *g = content->groups[frame];
                        int i;
                        for_i(i, g->buffers->len) {
                                struct device_buffer *db = array_get(g->buffers, struct device_buffer *, i);
                                id<MTLBuffer> buffer = (__bridge id _Nonnull)(db->ptr);
                                [encoder setVertexBuffer:buffer offset:0 atIndex:i];
                        }
                        struct device_buffer *udb = queue->pipeline->uniforms[frame];
                        id<MTLBuffer> buffer = (__bridge id _Nonnull)(udb->ptr);
                        [encoder setVertexBuffer:buffer offset:0 atIndex:i];
                        i++;
                        [encoder setFragmentBuffer:buffer offset:0 atIndex:0];

                        /* bind textures */
                        struct texture **tex;
                        array_for_each_index(tex, i, content->textures) {
                                if(*tex) {
                                        id<MTLTexture> texture = (__bridge id _Nonnull)((*tex)->ptr);
                                        [encoder setFragmentTexture:texture atIndex:i];
                                }
                        }

                        [encoder setFragmentSamplerState:sampler atIndex:0];

                        [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                                vertexStart:0 vertexCount:content->vertice
                                instanceCount: (content->current_instances * content->instance_multiple)
                        ];
                }
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
        setup();
        encoder = [shared_mtl_command_buffer renderCommandEncoderWithDescriptor:(__bridge id _Nonnull)(p->pass->ptr)];
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

        [encoder endEncoding];
        encoder = nil;
}

#endif
