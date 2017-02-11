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

#define int_to_chars(var,i) \
  char var[10]; \
  snprintf(var, sizeof(var), "%d", i);

#define int_to_chars_style(var,i,style) \
  char var[10]; \
  snprintf(var, sizeof(var), style, i);

#endif
