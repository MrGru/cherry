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
#include <cherry/memory.h>
#include <cherry/server/file_descriptor.h>
#include <cherry/server/web_server.h>
#include <cherry/array.h>
#include <cherry/stdio.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/stdlib.h>
#include <unistd.h>

#define DEFAULT_PORT 50000

int main(int argc, char* argv[])
{
        struct string *root     = string_alloc(0);
        u16 port                = DEFAULT_PORT;
        char *r                 = getenv("PWD");
        string_cat(root, r, strlen(r));

        struct web_server *ws   = web_server_alloc();

        char c;
        while ((c = getopt (argc, argv, "p:r:")) != -1) {
                switch (c) {
                        case 'r':
                                root->len = 0;
                                string_cat(root, optarg, strlen(optarg));
                                break;
                        case 'p':
                                port = atoi(optarg);
                                break;
                        case '?':
                                fprintf(stderr,"Wrong arguments given!!!\n");
                                goto finish;
                        default:
                                goto finish;
                }
        }
        web_server_start(ws, port, root->ptr);
        /* destroy cache and free memory pages allocated */
finish:;
        string_free(root);
        web_server_free(ws);
        cache_free();
        dim_memory();
        return 0;
}
