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
#import <cherry/platform.h>

#if OS == IOS

#import <cherry/stdio.h>
#import <cherry/string.h>
#import <Foundation/Foundation.h>

FILE *file_open(char *file, char *mode)
{
        FILE* p = fopen([[[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file]
                ofType:nil inDirectory:nil] UTF8String], mode);
        return p;
}

struct string *file_full_path(char *file)
{
        NSString *s = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file]
                ofType:nil inDirectory:nil];
        return string_alloc_chars([s UTF8String], s.length);
}

#endif
