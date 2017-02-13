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
