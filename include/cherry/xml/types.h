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
#ifndef __CHERRY_XML_TYPES_H__
#define __CHERRY_XML_TYPES_H__

#include <cherry/types.h>

struct xml_attribute {
        struct list_head        head;

        struct string           *name;
        struct string           *value;
};

struct xml_element {
        struct string           *name;
        struct string           *value;

        struct list_head        attributes;

        struct list_head        children;
        struct list_head        element_head;

        struct xml_element      *parent;
};

#endif