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
#import <cherry/graphic/shader.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/array.h>
#import <cherry/string.h>
#import <cherry/stdio.h>
#import <cherry/graphic/device_buffer.h>

/*
 * shader_house to keep device mtl pipelines alive
 */
static NSMutableArray *shader_house = nil;

struct shader *shader_alloc(char* vert, char* frag, struct shader_descriptor *des)
{
 	if(shader_house == nil) {
 		shader_house = [[NSMutableArray alloc] init];
        }
        /* create pipeline fragment program */
	id<MTLFunction> fragmentProgram = [shared_mtl_library newFunctionWithName:[NSString stringWithUTF8String:frag]];
	if(!fragmentProgram)
                debug(">> ERROR: Couldn't load fragment function from default library");

        /* create pipeline vertex program */
	id<MTLFunction> vertexProgram = [shared_mtl_library newFunctionWithName:[NSString stringWithUTF8String:vert]];
 	if(!vertexProgram)
                debug(">> ERROR: Couldn't load vertex function from default library");

  	/* create pipeline descriptor */
	MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];

	pipelineStateDescriptor.sampleCount                     = shared_mtl_sample_count;
	pipelineStateDescriptor.vertexFunction                  = vertexProgram;
	pipelineStateDescriptor.fragmentFunction                = fragmentProgram;
	pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineStateDescriptor.colorAttachments[0].blendingEnabled = YES;
        pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
	pipelineStateDescriptor.depthAttachmentPixelFormat      = MTLPixelFormatDepth32Float;

	NSError *error = nil;
	id <MTLRenderPipelineState> pipelineState = [shared_mtl_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
	if(!pipelineState) {
	    return NULL;
	}

	[shader_house addObject:pipelineState];

        /* create application shader object */
	struct shader *p = smalloc(sizeof(struct shader));
	p->ptr = (__bridge void *)(pipelineState);

	return p;
}

/*
 * deallocate shader object
 */
void shader_free(struct shader *p)
{
        if(p->ptr) {
                [shader_house removeObject:(__bridge id _Nonnull)(p->ptr)];
		p->ptr = NULL;
        }
        sfree(p);
}
