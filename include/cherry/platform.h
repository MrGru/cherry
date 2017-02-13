/*
 * platform definition
 */
#ifndef __CHERRY_PLATFORM_H__
#define __CHERRY_PLATFORM_H__

#define UNKNOWN 0
#define IOS 1
#define DROID 2
#define LINUX 3
#define OSX 4

#define OS DROID

#if defined(__APPLE__)

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
