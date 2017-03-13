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

struct device_buffer *device_buffer_alloc(u8 type, u16 item_size, u8 location)
{
        if(buffer_house == nil) {
		buffer_house = [[NSMutableArray alloc] init];
	}
        struct device_buffer *p = smalloc(sizeof(struct device_buffer));
        p->ptr                  = NULL;
        p->type                 = type;
        p->size                 = 0;
        p->ref                  = 0;
        p->item_size            = item_size;
        p->location             = location;
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
        p->ref--;
        if(p->ref <= 0) {
                if(p->ptr) {
        		[buffer_house removeObject:(__bridge id _Nonnull)(p->ptr)];
        		p->ptr = NULL;
        	}
                sfree(p);
        }
}

/*
 * allocate new device buffer group
 */
struct device_buffer_group *device_buffer_group_alloc()
{
        struct device_buffer_group *p = smalloc(sizeof(struct device_buffer_group));
        p->buffers = array_alloc(sizeof(struct device_buffer *), ORDERED);
        return p;
}

void device_buffer_group_add(struct device_buffer_group *g, struct device_buffer *b)
{
        b->ref++;
        array_push(g->buffers, &b);
}

/*
 * deallocate device buffer group
 */
void device_buffer_group_free(struct device_buffer_group *p)
{
        array_deep_free(p->buffers, struct device_buffer *, device_buffer_free);
        sfree(p);
}

#endif
