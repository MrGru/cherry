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
#import <cherry/graphic/image.h>

#if OS == MTL && IMAGE_TYPE == IMAGE_OBJC
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/stdio.h>
#import <cherry/string.h>
#import <UIKit/UIKit.h>

static void dataImage(struct image *p, UIImage *uiimage)
{
    CGImageRef imageRef = [uiimage CGImage];

    // Create a suitable bitmap context for extracting the bits of the image
    const NSUInteger width = CGImageGetWidth(imageRef);
    const NSUInteger height = CGImageGetHeight(imageRef);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    uint8_t *rawData = (uint8_t *)malloc(height * width * 4);
    const NSUInteger bytesPerPixel = 4;
    const NSUInteger bytesPerRow = bytesPerPixel * width;
    const NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(rawData, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

    CGContextTranslateCTM(context, 0, height);
    CGContextScaleCTM(context, 1, -1);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(context);

    p->ptr = rawData;
    p->bytes_per_row = bytesPerRow;
}

/*
 * allocate new image object from file path
 */
struct image *image_alloc(char *file)
{
        /* create objc image */
        struct string *full_path = file_full_path(file);
        UIImage *uiimage = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:full_path->ptr]];
        string_free(full_path);

        /* create application image */
        struct image *ret = smalloc(sizeof(struct image));
        ret->width = uiimage.size.width * uiimage.scale;
        ret->height = uiimage.size.height * uiimage.scale;
        dataImage(ret, uiimage);
        ret->type = MTLPixelFormatRGBA8Unorm;
        return ret;
}

/*
 * deallocate image object
 */
void image_free(struct image *p)
{
        free(p->ptr);
        sfree(p);
}

/*
 * get image width
 */
u16 image_width(struct image *p)
{
        return p->width;
}

/*
 * get image height
 */
u16 image_height(struct image *p)
{
        return p->height;
}

/*
 * get image pixels
 */
void *image_pixels(struct image *p)
{
        return p->ptr;
}

/*
 * get image type
 */
i32 image_type(struct image *p)
{
        return p->type;
}

#endif
