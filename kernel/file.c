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
#include <cherry/platform.h>
#include <cherry/stdio.h>
#include <cherry/string.h>

#define FILE_READ_BUFF 4096

struct string *file_read_string(char *file)
{
        FILE *f = file_open(file, "r");
        if(!f) {
                debug(": can not open file or file does not exist!\n  path=%s\n",file);
                return NULL;
        }

        struct string *p = string_alloc(0);
        char buff[FILE_READ_BUFF];
        i32 count = 0;

        while((count = fread(buff, 1, FILE_READ_BUFF, f))) {
                count -= count == FILE_READ_BUFF ? 0 : 1;
                string_cat(p, buff, count);
        }

        fclose(f);
        return p;
}
