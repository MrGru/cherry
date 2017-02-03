#include <cherry/graphic/graphic.h>

#if TARGET_RENDERER == RENDERER_METAL

int opengl_max_textures         = 8;
int depth_texture_enable        = 1;
int video_width                 = 800;
int video_height                = 480;
int shadow_width                = 1024;

id<MTLDevice> shared_mtl_device = nil;
id<MTLLibrary> shared_mtl_library = nil;
id<MTLCommandBuffer> shared_mtl_command_buffer = nil;
MTLRenderPassDescriptor* shared_mtl_main_pass = nil;
MTLPixelFormat shared_mtl_pixel_format = MTLPixelFormatBGRA8Unorm;
NSUInteger shared_mtl_sample_count = 1;

#endif
