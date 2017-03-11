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
#ifndef __CHERRY_SERVER_WEB_SERVER_H__
#define __CHERRY_SERVER_WEB_SERVER_H__

#include <cherry/server/types.h>

struct web_server *web_server_alloc();

void web_server_start(struct web_server *p, u16 port, char *root);

void web_server_free(struct web_server *p);

#endif
