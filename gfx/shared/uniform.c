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
#include <cherry/graphic/uniform.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/bytes.h>

struct uniform_buffer *camera_uniform_buffer_alloc()
{
        struct uniform_buffer *p        = smalloc(sizeof(struct uniform_buffer));
        p->data                         = smalloc(sizeof(struct camera_uniform));
        p->types                        = smalloc(sizeof(union camera_uniform_type));
        p->update_indice                = array_alloc(sizeof(u8), UNORDERED);
        union camera_uniform_type *t    = p->types;
        t->project.mask         = -1;
        t->project.type         = UNIFORM_M4;
        t->project.offset       = offsetof(struct camera_uniform, project);
        t->project.frame        = 0;
        t->view.mask            = -1;
        t->view.type            = UNIFORM_M4;
        t->view.offset          = offsetof(struct camera_uniform, view);
        t->view.frame           = 0;
        t->position.mask        = -1;
        t->position.type        = UNIFORM_F3;
        t->position.offset      = offsetof(struct camera_uniform, position);
        t->position.frame       = 0;
        uniform_buffer_init(p, sizeof(struct camera_uniform));
        return p;
}

struct uniform_buffer *transform_uniform_buffer_alloc()
{
        struct uniform_buffer *p        = smalloc(sizeof(struct uniform_buffer));
        p->data                         = smalloc(sizeof(struct transform_uniform));
        p->types                        = smalloc(sizeof(union transform_uniform_type));
        p->update_indice                = array_alloc(sizeof(u8), UNORDERED);
        union transform_uniform_type *t = p->types;
        t->transform.mask               = -1;
        t->transform.type               = UNIFORM_M4;
        t->transform.offset             = offsetof(struct transform_uniform, transform);
        t->transform.frame              = 0;
        uniform_buffer_init(p, sizeof(struct transform_uniform));
        return p;
}

struct uniform_buffer *light_uniform_buffer_alloc()
{
        struct uniform_buffer *p        = smalloc(sizeof(struct uniform_buffer));
        // uniform_buffer_init(p, sizeof());
        return p;
}

void uniform_buffer_set(struct uniform_buffer *p, u8 index, void *data, size_t len)
{
        p->update                       = 1;
        struct uniform_type *types      = p->types;
        if(types[index].frame == 0) {
                array_push(p->update_indice, &index);
        }
        smemcpy(p->data + types[index].offset, data, len);
        types[index].frame              = BUFFERS;
        types[index].mask               = -1;
}
