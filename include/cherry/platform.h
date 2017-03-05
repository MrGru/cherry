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
#ifndef __CHERRY_PLATFORM_H__
#define __CHERRY_PLATFORM_H__

#define UNKNOWN 0
#define IOS 1
#define DROID 2
#define LINUX 3
#define OSX 4
#define WEB 5

#define OS DROID

#if defined(EMSCRIPTEN)
        #undef OS
        #define OS WEB
#elif defined(__APPLE__)

        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
        #elif TARGET_OS_IPHONE

                #undef OS
                #define OS IOS

        #elif TARGET_OS_MAC
                #undef OS
                #define OS OSX
        #endif

#elif defined(ANDROID)

        #undef OS
        #define OS DROID

#elif defined(linux)

        #undef OS
        #define OS LINUX

#endif

#endif
