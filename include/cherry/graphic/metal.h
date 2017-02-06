#import <Metal/Metal.h>

/* whole game uses single mtl device */
extern id<MTLDevice> shared_mtl_device;
/* shader generator */
extern id<MTLLibrary> shared_mtl_library;

extern id<MTLCommandBuffer> shared_mtl_command_buffer;

extern MTLRenderPassDescriptor* shared_mtl_main_pass;

extern MTLPixelFormat shared_mtl_pixel_format;

extern NSUInteger shared_mtl_sample_count;
