#ifndef __CHERRY_STDIO_H__
#define __CHERRY_STDIO_H__

#include <stdio.h>

#ifndef APP_DEBUG
        #define __debug(f_, ...)
#else
        #if APP_DEBUG != 0
                #define __debug(f_, ...) printf(f_, __VA_ARGS__)
        #else
                #define __debug(f_, ...)
        #endif
#endif

#endif
