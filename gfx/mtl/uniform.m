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
#import <cherry/graphic/uniform.h>

#if GFX == MTL
#import <cherry/graphic/metal.h>
#import <cherry/memory.h>
#import <cherry/array.h>
#import <cherry/bytes.h>
#import <cherry/stdio.h>
#import <cherry/graphic/device_buffer.h>

void uniform_buffer_init(struct uniform_buffer *p, size_t size)
{
        int i;
        for_i(i, BUFFERS) {
                p->buffers[i] = device_buffer_alloc(BUFFER_VERTICE, size, BUFFER_PINNED);
                device_buffer_fill(p->buffers[i], p->data, size);
                p->buffers[i]->ref++;
        }
}

void uniform_buffer_update(struct uniform_buffer *p, u8 frame)
{
        struct uniform_type *types      = p->types;
        u8 *it;
        i16 it_index;
        array_for_each_index(it, it_index, p->update_indice) {
                u8 index                = *it;
                if(types[index].mask == frame) continue;

                types[index].frame--;
                types[index].mask       = frame;

                if(types[index].frame == 0) {
                        array_remove(p->update_indice, it_index);
                        it_index--;
                        it--;
                }

                void *data              = p->data + types[index].offset;
                switch (types[index].type) {
                        case UNIFORM_F1:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(float));
                                break;
                        case UNIFORM_F2:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(float) * 2);
                                break;
                        case UNIFORM_F3:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(float) * 3);
                                break;
                        case UNIFORM_F4:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(float) * 4);
                                break;
                        case UNIFORM_I1:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(int));
                                break;
                        case UNIFORM_I2:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(int) * 2);
                                break;
                        case UNIFORM_I3:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(int) * 3);
                                break;
                        case UNIFORM_I4:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(int) * 4);
                                break;
                        case UNIFORM_M2:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(union mat2));
                                break;
                        case UNIFORM_M3:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(union mat3));
                                break;
                        case UNIFORM_M4:
                                device_buffer_sub(p->buffers[frame], types[index].offset, data, sizeof(union mat4));
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
        int i;
        for_i(i, BUFFERS) {
                device_buffer_free(p->buffers[i]);
        }
        sfree(p);
}

#endif
