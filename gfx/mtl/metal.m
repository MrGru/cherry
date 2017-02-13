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
#include <cherry/graphic/graphic.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
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
