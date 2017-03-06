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
#include<sys/select.h>
#include <cherry/memory.h>
#include <cherry/server/file_descriptor.h>
#include <cherry/array.h>
#include <cherry/stdio.h>

int main(void)
{
        struct file_descriptor_set *fds = file_descriptor_set_alloc();
        file_descriptor_set_add(fds, 6);
        file_descriptor_set_add(fds, 20);
        file_descriptor_set_add(fds, 33);
        file_descriptor_set_add(fds, 1100);
        struct array *actives = array_alloc(sizeof(u32), ORDERED);
        file_descriptor_get_active(fds, actives);
        u32 *a;
        array_for_each(a, actives) {
                debug("%d\n", *a);
        }
        file_descriptor_set_free(fds);
        array_free(actives);
        /* destroy cache and free memory pages allocated */
        cache_free();
        dim_memory();
        return 0;
}
