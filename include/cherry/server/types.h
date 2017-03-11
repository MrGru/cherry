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
#ifndef __CHERRY_SERVER_TYPES_H__
#define __CHERRY_SERVER_TYPES_H__

#include <cherry/types.h>

struct file_descriptor_set {
        struct array    *set;
};

struct web_server {
        struct file_descriptor_set      *master;
        struct file_descriptor_set      *incomming;
        int                             fdmax;
        int                             listener;
        struct string                   *root;
};

#endif
