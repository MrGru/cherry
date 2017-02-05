#ifndef __CHERRY_STDIO_H__
#define __CHERRY_STDIO_H__

#include <stdio.h>
#include <cherry/types.h>

#ifndef APP_DEBUG
        #define debug(...)
#else
        #if APP_DEBUG != 0
                #define debug(...) printf(__VA_ARGS__)
        #else
                #define debug(...)
        #endif
#endif

struct string *file_read_string(char *file);
struct string *file_full_path(char *file);
FILE *file_open(char *file, char *mode);

#endif
