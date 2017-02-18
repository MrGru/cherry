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
#ifndef __CHERRY_XML_XML_H__
#define __CHERRY_XML_XML_H__

#include <cherry/xml/types.h>

struct xml_element *xml_parse(char *file);

void xml_free(struct xml_element *p);

struct xml_element *xml_find(struct xml_element* root, char* name, int index);

struct xml_attribute *xml_find_attribute(struct xml_element *root, char *name);

struct xml_element *xml_find_deep(struct xml_element *root, char *name, char *attr, char *val);

#endif
