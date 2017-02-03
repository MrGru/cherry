#include <cherry/graphic/graphic.h>

#if TARGET_RENDERER == RENDERER_METAL

int opengl_max_textures         = 8;
int depth_texture_enable        = 1;
int video_width                 = 800;
int video_height                = 480;
int shadow_width                = 1024;

/*
 * application needs set these objects
 */
id<MTLDevice> shared_mtl_device = nil;

id<MTLLibrary> shared_mtl_library = nil;

id<MTLCommandBuffer> shared_mtl_command_buffer = nil;

MTLRenderPassDescriptor* shared_mtl_main_pass = nil;

MTLPixelFormat shared_mtl_pixel_format = MTLPixelFormatBGRA8Unorm;

NSUInteger shared_mtl_sample_count = 1;

#include <cherry/memory.h>
#include <cherry/graphic/device_buffer.h>

/*
 * device buffer implementation
 */
static NSMutableArray *buffer_house = nil;

struct device_buffer_head *device_buffer_alloc()
{
        if(buffer_house == nil) {
		buffer_house = [NSMutableArray array];
	}
        struct device_buffer_head *head = smalloc(sizeof(struct device_buffer_head));
        head->ptr = NULL;
        head->size = 0;
        return head;
}

void device_buffer_fill(struct device_buffer_head *head, void *bytes, uint64_t size)
{
        if(head->size >= size) {
		id<MTLBuffer> buffer = (__bridge id _Nonnull)(head->ptr);
		void *mem = [buffer contents];
		smemcpy(mem, bytes, size);
	} else {
                if(head->ptr) [buffer_house removeObject:(__bridge id _Nonnull)(head->ptr)];
                id<MTLBuffer> buffer = [shared_mtl_device newBufferWithLength:size
                                   options:MTLResourceOptionCPUCacheModeDefault];
                void *mem = [buffer contents];
                smemcpy(mem, bytes, size);
                [buffer_house addObject:buffer];
                head->ptr = (__bridge void *)(buffer);
                head->size = size;
	}
}

void device_buffer_sub(struct device_buffer_head *head, uint64_t offset, void *bytes, uint64_t size)
{
        id<MTLBuffer> buffer = (__bridge id _Nonnull)(head->ptr);
        void *mem = [buffer contents];
        smemcpy(mem + offset, bytes, size);
}

void device_buffer_free(struct device_buffer_head *head)
{
        if(head->ptr) {
		[buffer_house removeObject:(__bridge id _Nonnull)(head->ptr)];
		head->ptr = NULL;
	}
        sfree(head);
}

#endif
