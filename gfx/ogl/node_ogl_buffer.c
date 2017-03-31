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
#include <cherry/graphic/node.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/string.h>
#include <cherry/bytes.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/device_buffer.h>

void node_setup_buffer(struct device_buffer *buffer, struct shader *s, u32 key)
{
        device_buffer_bind(buffer);
        GLint attr      = -1;
        switch (key) {
                case INPUT_POSITION_2D:
                        attr    = glGetAttribLocation((GLuint)s->id, "position");
                        if(attr == -1) break;
                        glEnableVertexAttribArray(attr);
                        glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, sizeof(union vec2), (void*)(0));
                        break;
                case INPUT_POSITION_3D:
                        attr    = glGetAttribLocation((GLuint)s->id, "position");
                        if(attr == -1) break;
                        glEnableVertexAttribArray(attr);
                        glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, sizeof(union vec3), (void*)(0));
                        break;
                case INPUT_NORMAL:
                        attr    = glGetAttribLocation((GLuint)s->id, "normal");
                        if(attr == -1) break;
                        glEnableVertexAttribArray(attr);
                        glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, sizeof(union vec3), (void*)(0));
                        break;
                case INPUT_TEXCOORD:
                        attr    = glGetAttribLocation((GLuint)s->id, "texcoord");
                        if(attr == -1) break;
                        glEnableVertexAttribArray(attr);
                        glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, sizeof(union vec2), (void*)(0));
                        break;
                case INPUT_COLOR:
                        attr    = glGetAttribLocation((GLuint)s->id, "color");
                        if(attr == -1) break;
                        glEnableVertexAttribArray(attr);
                        glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, sizeof(union vec4), (void*)(0));
                        break;
                default:
                        break;
        }
}

#endif
