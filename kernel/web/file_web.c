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

#if OS == WEB

#include <cherry/stdio.h>
#include <cherry/string.h>

FILE *file_open(char *file, char *mode)
{
        return fopen(file, mode);
}

struct string *file_full_path(char *file)
{
        return string_alloc_chars(file, strlen(file));
}

#endif
