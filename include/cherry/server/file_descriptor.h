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
#ifndef __CHERRY_SERVER_FILE_DESCRIPTOR_H__
#define __CHERRY_SERVER_FILE_DESCRIPTOR_H__

#include <cherry/server/types.h>

struct file_descriptor_set *file_descriptor_set_alloc();

void file_descriptor_set_free(struct file_descriptor_set *p);

void file_descriptor_set_add(struct file_descriptor_set *p, u32 fd);

void file_descriptor_set_remove(struct file_descriptor_set *p, u32 fd);

void file_descriptor_clean(struct file_descriptor_set *p);

void file_descriptor_get_active(struct file_descriptor_set *p, struct array *o);

#endif
