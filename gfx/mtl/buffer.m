#import <cherry/graphic/graphic.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/graphic/device_buffer.h>
#import <cherry/array.h>
/*
 * buffer_house to keep device mtl buffers alive
 */
static NSMutableArray *buffer_house = nil;

struct device_buffer *device_buffer_alloc()
{
        if(buffer_house == nil) {
		buffer_house = [NSMutableArray array];
	}
        struct device_buffer *p = smalloc(sizeof(struct device_buffer));
        p->ptr = NULL;
        p->size = 0;
        return p;
}

void device_buffer_fill(struct device_buffer *p, void *bytes, u32 size)
{
        if(p->size >= size) {
		id<MTLBuffer> buffer = (__bridge id _Nonnull)(p->ptr);
		void *mem = [buffer contents];
		smemcpy(mem, bytes, size);
	} else {
                if(p->ptr) [buffer_house removeObject:(__bridge id _Nonnull)(p->ptr)];
                id<MTLBuffer> buffer = [shared_mtl_device newBufferWithLength:size
                                   options:MTLResourceOptionCPUCacheModeDefault];
                void *mem = [buffer contents];
                smemcpy(mem, bytes, size);
                [buffer_house addObject:buffer];
                p->ptr = (__bridge void *)(buffer);
                p->size = size;
	}
}

void device_buffer_sub(struct device_buffer *p, u32 offset, void *bytes, u32 size)
{
        id<MTLBuffer> buffer = (__bridge id _Nonnull)(p->ptr);
        void *mem = [buffer contents];
        smemcpy(mem + offset, bytes, size);
}

void device_buffer_free(struct device_buffer *p)
{
        if(p->ptr) {
		[buffer_house removeObject:(__bridge id _Nonnull)(p->ptr)];
		p->ptr = NULL;
	}
        sfree(p);
}

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc()
{
        struct device_buffer_group *p = smalloc(sizeof(struct device_buffer_group));
        p->buffers = array_alloc(sizeof(struct device_buffer *), ORDERED);
        p->pipeline = NULL;
        return p;
}

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p)
{
        array_free(p->buffers);
        sfree(p);
}

#endif
