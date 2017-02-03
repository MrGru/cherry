/*
 * platform definition
 */
#ifndef __CHERRY_PLATFORM_H__
#define __CHERRY_PLATFORM_H__

#define RENDERER_UNKNOWN 0
#define RENDERER_OPENGL 1
#define RENDERER_METAL 2

#ifndef TARGET_RENDERER
#define TARGET_RENDERER RENDERER_UNKNOWN
#endif

#define PLATFORM_UNKNOWN 0
#define PLATFORM_IOS 1
#define PLATFORM_ANDROID 2
#define PLATFORM_LINUX 3
#define PLATFORM_MAC 4

#define TARGET_PLATFORM PLATFORM_IOS

#if defined(__APPLE__)

  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR
  #elif TARGET_OS_IPHONE

    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_IOS

  #elif TARGET_OS_MAC
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM PLATFORM_MAC
  #endif

#elif defined(linux)

  #undef TARGET_PLATFORM
  #define TARGET_PLATFORM PLATFORM_LINUX

#endif

#endif
