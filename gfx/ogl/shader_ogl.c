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
#include <cherry/graphic/shader.h>

#if GFX == OGL
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/graphic/link_uniform.h>

static struct shader *current_shader = NULL;

struct string *shader_read_file(char *file)
{
        struct string *content  = file_read_string(file);
        struct array *ocs       = array_alloc(sizeof(u32), ORDERED);
        string_find(content, "#include", ocs);

        struct string *re_path  = string_alloc(0);
        u32 file_len            = strlen(file);

        file_len--;
        while(file_len) {
                if(file[file_len] == '/') {
                        string_cat(re_path, file, file_len + 1);
                        break;
                }
                file_len--;
        }

        struct string *lib_path = string_alloc(0);
        i16 i;
        for(i = ocs->len - 1; i >= 0; i-= 2) {
                u32 start       = array_get(ocs, u32, i - 1);
                u32 end         = array_get(ocs, u32, i);
                u32 range       = 0;
                lib_path->len   = 0;
                string_cat_string(lib_path, re_path);
                u32 slib_id     = 0;
                u32 j;
                for(j = end + 1; j < content->len; j++) {
                        if(content->ptr[j] == '<') {
                                slib_id = j + 1;
                        } else if(content->ptr[j] == '>') {
                                range = j - start + 1;
                                string_cat(lib_path, content->ptr + slib_id, j - slib_id);
                                goto found;
                        }
                }
        notfound:
                continue;
        found:;
                struct string *lib_content = file_read_string(lib_path->ptr);
                string_replace_direct(content, start, range, lib_content->ptr, lib_content->len);
                string_free(lib_content);
        }

        string_free(re_path);
        string_free(lib_path);
        array_free(ocs);
        return content;
}

struct shader *shader_alloc(char *vert, char *frag)
{
        /* compile vertex shader */
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, (const GLchar**)&vert, NULL);
        glCompileShader(vertexShader);
        /* check for vertex shader compilation error */
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            debug("compile shader vertex error:\n%s\n", infoLog);
            return NULL;
        }
        /* compile fragment shader */
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, (const GLchar**)&frag, NULL);
        glCompileShader(fragmentShader);
        /* check for fragment shader compilation error */
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            debug("compile shader fragment error:\n%s\n", infoLog);
            return NULL;
        }
        /* link vertex shader and fragment shader together */
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        /* check for linking error */
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            debug("link shader error:\n%s\n", infoLog);
            return NULL;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        /* allocate application shader object */
        struct shader *p        = smalloc(sizeof(struct shader));
        p->id                   = shaderProgram;
        p->link_uniforms        = array_alloc(sizeof(struct link_uniform *), ORDERED);

        return p;
}

void shader_free(struct shader *p)
{
        if(p->id) {
                /* clear current_shader for other shaders to use */
                if(p == current_shader) {
                        current_shader = NULL;
                }
                glDeleteProgram(p->id);
        }
        array_deep_free(p->link_uniforms, struct link_uniform *, link_uniform_free);
        sfree(p);
}

void shader_use(struct shader *p)
{
        if(p != current_shader) {
                current_shader = p;
                glUseProgram(p->id);
        }
}

#endif
