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
#include <cherry/xml/xml.h>
#include <cherry/stdio.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/ctype.h>
#include <cherry/list.h>

#define xml_none                        0
#define xml_read_element_name           1
#define xml_find_attribute_name         2
#define xml_read_attribute_name         3
#define xml_find_attribute_value        4
#define xml_read_attribute_value        5
#define xml_find_element                6
#define xml_check_element_value         7
#define xml_read_comment                8

static inline struct xml_attribute *__xml_attribute_alloc()
{
        struct xml_attribute *p = smalloc(sizeof(struct xml_attribute));
        INIT_LIST_HEAD(&p->head);
        p->name = string_alloc(0);
        p->value = string_alloc(0);
        return p;
}

static inline void __xml_attribute_free(struct xml_attribute *p)
{
        string_free(p->name);
        string_free(p->value);
        list_del(&p->head);
        sfree(p);
}

static inline struct xml_element *__xml_element_alloc()
{
        struct xml_element *p = smalloc(sizeof(struct xml_element));
        p->name = string_alloc(0);
        p->value = string_alloc(0);
        INIT_LIST_HEAD(&p->attributes);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->element_head);
        p->parent = NULL;
        return p;
}
static inline void __xml_element_free(struct xml_element *p)
{
        string_free(p->name);
        string_free(p->value);
        struct list_head *head;
        list_while_not_singular(head, &p->attributes) {
                struct xml_attribute *a = (struct xml_attribute *)
                        ((void *)head - offsetof(struct xml_attribute, head));
                __xml_attribute_free(a);
        }
        list_while_not_singular(head, &p->children) {
                struct xml_element *e = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                __xml_element_free(e);
        }
        list_del(&p->element_head);
        p->parent = NULL;
        sfree(p);
}

static inline void __xml_element_add_element(struct xml_element *p, struct xml_element *q)
{
        list_del(&q->element_head);
        list_add_tail(&q->element_head, &p->children);
        q->parent = p;
}

static inline void __xml_element_add_attribute(struct xml_element *p, struct xml_attribute *a)
{
        list_del(&a->head);
        list_add_tail(&a->head, &p->attributes);
}

struct xml_element *xml_parse(char *file)
{
        struct string *text     = file_read_string(file);
        int depth               = 0;
        u32 i;
        u32 start               = 0;
        u32 end                 = 0;
        u8  finish              = 0;
        u8  empty_text          = 1;
        u8  state               = xml_none;

        struct xml_element *host        = __xml_element_alloc();
        struct xml_element *current     = host;
        struct xml_attribute *attr      = NULL;

        int head_size = 1;
        for_i_step(i, head_size, text->len) {
                char c = text->ptr[i];
                head_size = __check_utf8_bytes(&c);
                switch(state) {
                        case xml_read_comment:
                                if(c == '-'
                                        && i + 1 < text->len
                                        && i + 2 < text->len
                                        && text->ptr[i + 1] == '-'
                                        && text->ptr[i + 2] == '>') {
                                        start   = end = 0;
                                        finish  = 0;
                                        state   = xml_none;
                                }
                                break;
                        case xml_none :
                                if(c == '<') {
                                        if(text->ptr[i + 1] == '!') {
                                                state = xml_read_comment;
                                        } else if(text->ptr[i + 1] == '/') {
                                                i++;
                                                current = current->parent;
                                                state   = xml_none;
                                        } else {
                                                if(current) {
                                                        struct xml_element *child = __xml_element_alloc();
                                                        __xml_element_add_element(current, child);
                                                        current = child;
                                                } else {
                                                        current = __xml_element_alloc();
                                                        __xml_element_add_element(host, current);
                                                }
                                                start   = i + 1;
                                                end     = i;
                                                state   = xml_read_element_name;
                                                finish  = 0;
                                        }
                                }
                                break;
                        case xml_read_element_name :
                                if(isspace(c)) {
                                        finish  = 1;
                                        state   = xml_find_attribute_name;
                                } else {
                                        if(c == '>') {
                                                finish          = 1;
                                                state           = xml_check_element_value;
                                                empty_text      = 1;
                                        } else if (c == '/') {
                                                finish  = 1;
                                                state   = xml_none;
                                        } else {
                                                end += head_size;
                                        }
                                }
                                if(finish) {
                                        if(end >= start) {
                                                string_cat(current->name, text->ptr + start,
                                                        end - start + 1);
                                        }
                                        finish  = 0;
                                        start   = 0;
                                        end     = 0;
                                        if(state == xml_none) {
                                                current = current->parent;
                                        }
                                }
                                break;
                        case xml_find_attribute_name:
                                if(c == '>') {
                                        // finish read attributes, jump to check element value
                                        state           = xml_check_element_value;
                                        start           = end = 0;
                                        empty_text      = 1;
                                } else if(c == '/' || c == '?') {
                                        current = current->parent;
                                        state   = xml_none;
                                }  else if(!isspace(c)) {
                                        //begin read attribute name
                                        state   = xml_read_attribute_name;
                                        start   = i;
                                        end     = i;
                                        finish  = 0;
                                }
                                break;
                        case xml_read_attribute_name:
                                if(isspace(c) || c == '=') {
                                        //finish read attribute name
                                        attr = __xml_attribute_alloc();
                                        string_cat(attr->name, text->ptr + start, end - start + 1);
                                        __xml_element_add_attribute(current, attr);
                                        start   = end = 0;
                                        finish  = 0;
                                        state   = xml_find_attribute_value;
                                } else {
                                        end += head_size;
                                }
                                break;
                        case xml_find_attribute_value:
                                if(c == '>') {
                                        // finish read attributes, jump to check element value
                                        state           = xml_check_element_value;
                                        start           = end = 0;
                                        empty_text      = 1;
                                } else if(c == '?') {
                                        current = current->parent;
                                        state   = xml_none;
                                } else if(c == '\"' || c == '\'') {
                                        //begin read attribute name
                                        state   = xml_read_attribute_value;
                                        start   = i + 1;
                                        end     = i;
                                        finish  = 0;
                                }
                                break;
                        case xml_read_attribute_value:
                                if(c == '\"' || c == '\'') {
                                        //finish read attribute name
                                        if(end >= start)
                                                string_cat(attr->value,
                                                        text->ptr + start, end - start + 1);
                                        start   = end = 0;
                                        finish  = 0;
                                        state   = xml_find_attribute_name;
                                } else {
                                        end += head_size;
                                }
                               break;
                          case xml_find_element:
                                if(c == '/') {
                                        //find end element
                                        current = current->parent;
                                        state   = xml_none;
                                } else if(c == '!') {
                                        state = xml_read_comment;
                                } else {
                                        /* create new child and assign it to current parent */
                                        start   = end = i;
                                        finish  = 0;
                                        state   = xml_read_element_name;
                                        struct xml_element* child = __xml_element_alloc();
                                        __xml_element_add_element(current, child);
                                        current = child;
                                }
                                break;
                        case xml_check_element_value:
                                if(c == '<') {
                                        //finish read element value
                                        if(!empty_text) {
                                                //element has value
                                                string_cat(current->value,
                                                        text->ptr + start, end - start + 1);
                                        }
                                        state   = xml_find_element;
                                        start   = end = 0;
                                        finish  = 0;
                                } else {
                                        if(start == 0)
                                                start = i;
                                        end = i;
                                        if(!isspace(c))
                                                empty_text = 0;
                                }
                                break;
                }
        }
        string_free(text);

        return host;
}

void xml_free(struct xml_element *p)
{
        __xml_element_free(p);
}

struct xml_element *xml_find(struct xml_element* root, char* name, int index)
{
        struct list_head *head = &root->element_head;
        while(head) {
                struct xml_element *e = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                if(strcmp(e->name->ptr, name) == 0) {
                        if(index == 0) return e;
                        index--;
                }
                /*
                 * invalidate head for next check
                 */
                head = NULL;
                if(!list_singular(&e->children)) {
                        /*
                         * check first child
                         */
                        head = e->children.next;
                } else {
                        /*
                         * check next sibling
                         * if e is last node than check next sibling of parent recursily
                         */
                        while(e->parent) {
                                if(e->element_head.next != &e->parent->children) {
                                        /*
                                         * e has next sibling
                                         */
                                        head = e->element_head.next;
                                        break;
                                } else {
                                        /*
                                         * e is last node, check for parent sibling
                                         */
                                        e = e->parent;
                                        /*
                                         * stop at root
                                         */
                                        if(e == root) break;
                                }
                        }
                }
        }
        return NULL;
}

static struct xml_attribute *__xml_find_attribute(struct xml_element *root, char *name)
{
        struct list_head *head;
        list_for_each(head, &root->attributes) {
                struct xml_attribute *a = (struct xml_attribute *)
                        ((void *)head - offsetof(struct xml_attribute, head));
                if(strcmp(a->name->ptr, name) == 0) return a;
        }
        return NULL;
}


struct xml_attribute *xml_find_attribute(struct xml_element *root, char *name)
{
        return __xml_find_attribute(root, name);
}

struct xml_element *xml_find_deep(struct xml_element *root, char *name, char *attr, char *val)
{
        struct list_head *head = &root->element_head;
        while(head) {
                struct xml_element *e = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                if(strcmp(e->name->ptr, name) == 0) {
                        struct xml_attribute *a = __xml_find_attribute(e, attr);
                        if(a && strcmp(a->value->ptr, val) == 0) {
                                return e;
                        }
                }
                /*
                 * invalidate head for next check
                 */
                head = NULL;
                if(!list_singular(&e->children)) {
                        /*
                         * check first child
                         */
                        head = e->children.next;
                } else {
                        /*
                         * check next sibling
                         * if e is last node than check next sibling of parent recursily
                         */
                        while(e->parent) {
                                if(e->element_head.next != &e->parent->children) {
                                        /*
                                         * e has next sibling
                                         */
                                        head = e->element_head.next;
                                        break;
                                } else {
                                        /*
                                         * e is last node, check for parent sibling
                                         */
                                        e = e->parent;
                                        /*
                                         * stop at root
                                         */
                                        if(e == root) break;
                                }
                        }
                }
        }
        return NULL;
}
