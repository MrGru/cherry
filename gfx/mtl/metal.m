#include <cherry/graphic/graphic.h>

#if GFX == MTL

/*
 * application needs set these objects
 */
id<MTLDevice> shared_mtl_device = nil;

id<MTLLibrary> shared_mtl_library = nil;

id<MTLCommandBuffer> shared_mtl_command_buffer = nil;

MTLRenderPassDescriptor* shared_mtl_main_pass = nil;

MTLPixelFormat shared_mtl_pixel_format = MTLPixelFormatBGRA8Unorm;

NSUInteger shared_mtl_sample_count = 1;

#endif
