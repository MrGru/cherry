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
#ifndef __CHERRY_GRAPHIC_METAL_H__
#define __CHERRY_GRAPHIC_METAL_H__
#import <Metal/Metal.h>

/* whole game uses single mtl device */
extern id<MTLDevice> shared_mtl_device;
/* shader generator */
extern id<MTLLibrary> shared_mtl_library;

extern id<MTLCommandBuffer> shared_mtl_command_buffer;

extern MTLRenderPassDescriptor* shared_mtl_main_pass;

extern MTLPixelFormat shared_mtl_pixel_format;

extern NSUInteger shared_mtl_sample_count;
#endif
