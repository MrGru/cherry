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
#import <cherry/graphic/node.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/list.h>
#import <cherry/array.h>
#import <cherry/map.h>
#import <cherry/string.h>
#import <cherry/bytes.h>
#import <cherry/math/math.h>
#import <cherry/stdio.h>
#import <cherry/graphic/shader.h>
#import <cherry/graphic/uniform.h>
#import <cherry/graphic/texture.h>
#import <cherry/graphic/device_buffer.h>
#import <cherry/graphic/render_pass.h>

static struct render_pass *current_pass                = NULL;
static id<MTLRenderCommandEncoder> encoder      = nil;
static id<MTLSamplerState>   sampler            = nil;
static struct shader *current_shader            = NULL;
static u8 current_blend                         = BLEND_NONE;

static void clear_cache()
{
        sampler = nil;
}

static void setup()
{
        if(sampler) return;

        cache_add(clear_cache);
        {
               MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
               samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
               samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
               samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
               samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

               sampler = [shared_mtl_device newSamplerStateWithDescriptor:samplerDescriptor];
       }
}

void render_pass_begin(struct render_pass *p, u8 frame)
{
        if(current_pass != p) {
                render_pass_end();
                current_pass = p;
        }
        if(encoder == nil) {
                encoder = [shared_mtl_command_buffer renderCommandEncoderWithDescriptor:(__bridge id _Nonnull)(current_pass->ptr)];
        }
}

void render_pass_end()
{
        if(current_pass) {
                [encoder endEncoding];
                encoder         = nil;
                current_pass    = NULL;
                current_shader  = NULL;
                current_blend   = BLEND_NONE;
        }
}

static void __blend(u8 mode, struct shader *pipeline)
{
        if(current_blend == mode && current_shader == pipeline) return;

        current_blend   = mode;
        current_shader  = pipeline;

        switch (current_blend) {
                case BLEND_NONE:
                        [encoder setRenderPipelineState:(__bridge id _Nonnull)(pipeline->ptr_none)];
                        break;
                case BLEND_ALPHA:
                        [encoder setRenderPipelineState:(__bridge id _Nonnull)(pipeline->ptr_alpha)];
                        break;
                case BLEND_MULTIPLY:
                        [encoder setRenderPipelineState:(__bridge id _Nonnull)(pipeline->ptr_multiply)];
                        break;
                case BLEND_ADDITIVE:
                        [encoder setRenderPipelineState:(__bridge id _Nonnull)(pipeline->ptr_additive)];
                        break;
                default:
                        [encoder setRenderPipelineState:(__bridge id _Nonnull)(pipeline->ptr_alpha)];
                        break;
        }
}

void node_render(struct node *p, u8 frame)
{
        setup();
        if(!current_pass || !p->visible) return;

        if(p->type == -1 || p->shader_type == -1) goto render_children;

        /*
         * update uniform
         */
        int i, j;

        /*
         * render
         */
        int start_index = 0;
        for_i(i, p->current_render_content[frame]->actives) {
                int si = start_index;
                struct node_render_buffer_group *group = array_get(p->current_render_content[frame]->buffer_groups,
                        struct node_render_buffer_group *, i);
                /*
                 * update blend
                 */
                __blend(group->blend_mode, p->current_shader);
                /*
                * draw
                 */
                for_i(j, group->group->buffers->len) {
                        struct device_buffer *db = array_get(group->group->buffers, struct device_buffer *, j);
                        id<MTLBuffer> buffer = (__bridge id _Nonnull)(db->ptr);
                        [encoder setVertexBuffer:buffer offset:0 atIndex:si];
                        si++;
                }

                int fi = 0;
                for(j = 0; j < p->current_common_uniform_buffers->len; si++, fi++, j++) {
                        struct uniform_buffer *ub       = array_get(p->current_common_uniform_buffers, struct uniform_buffer *, j);
                        uniform_buffer_update(ub, frame);
                        id<MTLBuffer> buffer = (__bridge id _Nonnull)(ub->buffers[frame]->ptr);
                        [encoder setVertexBuffer:buffer offset:0 atIndex:si];
                        [encoder setFragmentBuffer:buffer offset:0 atIndex:fi];
                }
                for(j = 0; j < p->current_uniform_buffers->len; si++, fi++, j++) {
                        struct uniform_buffer *ub       = array_get(p->current_uniform_buffers, struct uniform_buffer *, j);
                        uniform_buffer_update(ub, frame);
                        id<MTLBuffer> buffer = (__bridge id _Nonnull)(ub->buffers[frame]->ptr);
                        [encoder setVertexBuffer:buffer offset:0 atIndex:si];
                        [encoder setFragmentBuffer:buffer offset:0 atIndex:fi];
                }
                /*
                 * update textures
                 */
                struct texture **tex;
                array_for_each_index(tex, j, group->textures) {
                        id<MTLTexture> texture = (__bridge id _Nonnull)((*tex)->ptr);
                        [encoder setFragmentTexture:texture atIndex:j];
                }
                [encoder setFragmentSamplerState:sampler atIndex:0];

                [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                                vertexStart:0 vertexCount:group->vertice_count
                ];
        }

render_children:;
        struct list_head *head;
        list_for_each(head, &p->children) {
                struct node *n = (struct node *)
                        ((void *)head - offsetof(struct node, head));
                node_render(n, frame);
        }
}

#endif
