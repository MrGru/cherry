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

void uniform_buffer_init(struct uniform_buffer *p, size_t size)
{
        p->last_pipeline = NULL;
}

void uniform_buffer_force_update(struct uniform_buffer *p, struct link_uniform *link)
{
        struct uniform_type *types      = p->types;
        i32 *address_it;
        int index;
        array_for_each_index(address_it, index, link->ids) {
                void *data              = p->data + types[index].offset;
                i32 address             = *address_it;
                if(address < 0) continue;
                switch (types[index].type) {
                        case UNIFORM_F1:
                                glUniform1fv((GLint)address, 1, (GLfloat *)data);
                                break;
                        case UNIFORM_F2:
                                glUniform2fv((GLint)address, 1, (GLfloat *)data);
                                break;
                        case UNIFORM_F3:
                                glUniform3fv((GLint)address, 1, (GLfloat *)data);
                                break;
                        case UNIFORM_F4:
                                glUniform4fv((GLint)address, 1, (GLfloat *)data);
                                break;
                        case UNIFORM_I1:
                                glUniform1iv((GLint)address, 1, (GLint *)data);
                                break;
                        case UNIFORM_I2:
                                glUniform2iv((GLint)address, 1, (GLint *)data);
                                break;
                        case UNIFORM_I3:
                                glUniform3iv((GLint)address, 1, (GLint *)data);
                                break;
                        case UNIFORM_I4:
                                glUniform4iv((GLint)address, 1, (GLint *)data);
                                break;
                        case UNIFORM_M2:
                                glUniformMatrix2fv((GLint)address, 1, GL_FALSE, (GLfloat *)data);
                                break;
                        case UNIFORM_M3:
                                glUniformMatrix3fv((GLint)address, 1, GL_FALSE, (GLfloat *)data);
                                break;
                        case UNIFORM_M4:
                                glUniformMatrix4fv((GLint)address, 1, GL_FALSE, (GLfloat *)data);
                                break;
                        default:
                                break;
                }
        }
}

void uniform_buffer_free(struct uniform_buffer *p)
{
        sfree(p->types);
        sfree(p->data);
        array_free(p->update_indice);
        sfree(p);
}
