#import <cherry/graphic/shader.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/array.h>
#import <cherry/string.h>
#import <cherry/math/math.h>
#import <cherry/stdio.h>
#import <cherry/graphic/device_buffer.h>

/*
 * shader_house to keep device mtl pipelines alive
 */
static NSMutableArray *shader_house = nil;

struct shader *shader_alloc(char* vert, char* frag, struct shader_descriptor *des)
{
 	if(shader_house == nil) {
 		shader_house = [NSMutableArray array];
        }
        /* create piepline vertex descriptor from first application buffer descriptor*/
        MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor new];
        struct shader_buffer_descriptor *sbd = array_get(des->buffers, struct shader_buffer_descriptor *, 0);
        struct shader_attribute_descriptor **sad;
        i16 i;
        array_for_each_index(sad, i, sbd->attributes) {
                switch((*sad)->type) {
                        case ATTRIBUTE_FLOAT:
                                vertexDescriptor.attributes[i].format = MTLVertexFormatFloat;
                                break;
                        case ATTRIBUTE_VEC2:
                                vertexDescriptor.attributes[i].format = MTLVertexFormatFloat2;
                                break;
                        case ATTRIBUTE_VEC3:
                                vertexDescriptor.attributes[i].format = MTLVertexFormatFloat3;
                                break;
                        case ATTRIBUTE_VEC4:
                                vertexDescriptor.attributes[i].format = MTLVertexFormatFloat4;
                                break;
                        default:
                                break;
                }
                vertexDescriptor.attributes[i].offset = (*sad)->offset * sizeof(float);
                vertexDescriptor.attributes[i].bufferIndex = 0;
        }
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        vertexDescriptor.layouts[0].stride = sbd->vertex_size;

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
        pipelineStateDescriptor.vertexDescriptor                = vertexDescriptor;
	pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
	pipelineStateDescriptor.depthAttachmentPixelFormat      = MTLPixelFormatDepth32Float;

	NSError *error = nil;
	id <MTLRenderPipelineState> pipelineState = [shared_mtl_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
	if(!pipelineState) {
	    //NSLog(@">> ERROR: Failed Aquiring pipeline state: %@", error);
	    return NULL;
	}

	[shader_house addObject:pipelineState];

        /* create application shader object */
	struct shader *p = smalloc(sizeof(struct shader));
	p->ptr = (__bridge void *)(pipelineState);
	p->tracks = array_alloc(sizeof(struct shader_uniform_track *), ORDERED);
	p->flags = array_alloc(sizeof(i16), ORDERED);
	p->pendings = array_alloc(sizeof(i16), UNORDERED);
        p->descriptor = des;

        for_i(i, BUFFERS) {
                p->update[i] = 1;
                p->uniforms[i] = NULL;
        }
	return p;
}

/*
 * deallocate shader object
 */
void shader_free(struct shader *p)
{
        if(p->descriptor) shader_descriptor_free(p->descriptor);
        if(p->ptr) {
                [shader_house removeObject:(__bridge id _Nonnull)(p->ptr)];
		p->ptr = NULL;
        }
        int i;
        for_i(i, BUFFERS) {
                if(p->uniforms[i]) {
                        device_buffer_free(p->uniforms[i]);
                }
        }
        array_deep_free(p->tracks, struct shader_uniform_track *, shader_uniform_track_free);
        array_free(p->flags);
        array_free(p->pendings);
        sfree(p);
}

/*
 * reserve uniform
 * @index       : uniform global index
 * @type        : uniform type
 * @offset      : uniform offset
 */
void __shader_reserve_uniform(struct shader *p, i16 index, u8 type, i16 offset)
{
        if(index >= p->tracks->len) {
                while(index >= p->tracks->len) {
                        struct shader_uniform_track *t  = shader_uniform_track_alloc();
                        t->pipeline                     = p;
                        t->index                        = p->tracks->len;
                        if(index == t->index) {
                                t->type = type;
                                t->offset = offset;
                        }
                        array_push(p->tracks, &t);
                        array_push(p->flags, &(i16){0});
                }
        } else {
                struct shader_uniform_track *t = array_get(p->tracks, struct shader_uniform_track *, index);
                t->type = type;
                t->offset = offset;
        }
}

/*
 * update uniform datas
 * @p           : pipeline to manipulate
 * @frame       : current application frame
 */
void shader_update_uniform(struct shader *p, u8 frame)
{
        if(!p->pendings->len || !p->update[frame]) return;

        p->update[frame] = 0;

        i16 *pid;
        int i;
        array_for_each_index(pid, i, p->pendings) {
                struct shader_uniform_track *t = array_get(p->tracks, struct shader_uniform_track *, *pid);
                i16 n = array_get(p->flags, i16, *pid);
                n--;
                if(n == 0) {
                        /* last frame uniform has to update */
                        array_set(p->flags, *pid, &n);
                        array_remove(p->pendings, i);
                        i--;
                        pid--;
                } else {
                        array_set(p->flags, *pid, &n);
                }
                if(t->uniform) {
                        if(t->offset >= 0 && p->uniforms[frame]) {
                                device_buffer_sub(p->uniforms[frame], t->offset,
                                        t->uniform->data->ptr, t->uniform->data->len);
                        }
                }
        }
}

#endif
