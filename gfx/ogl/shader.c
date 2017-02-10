#include <cherry/graphic/shader.h>

#if GFX == OGL

#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/string.h>
#include <cherry/math/math.h>
#include <cherry/stdio.h>

static struct shader *current_shader = NULL;

/*
 * allocate new shader object
 */
struct shader *shader_alloc(char *vert, char *frag, struct shader_descriptor *des)
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
        p->tracks               = array_alloc(sizeof(struct shader_uniform_track *), ORDERED);
        p->flags                = array_alloc(sizeof(i16), ORDERED);
        p->pendings             = array_alloc(sizeof(i16), UNORDERED);
        p->texture_uniforms     = array_alloc(sizeof(struct shader_uniform *), ORDERED);
        p->descriptor           = des;
        int i;
        for_i(i, BUFFERS) p->update[i] = 1;

        return p;
}

/*
 * deallocate shader object
 */
void shader_free(struct shader *p)
{
        if(p->descriptor) shader_descriptor_free(p->descriptor);
        if(p->id) {
                /* clear current_shader for other shaders to use */
                if(p == current_shader) {
                        current_shader = NULL;
                }
                glDeleteProgram(p->id);
        }
        array_deep_free(p->tracks, struct shader_uniform_track *, shader_uniform_track_free);
        array_free(p->flags);
        array_free(p->pendings);
        array_deep_free(p->texture_uniforms, struct shader_uniform *, shader_uniform_free);
        sfree(p);
}

/*
 * reserve uniform
 * @index       : uniform global index
 * @type        : uniform type
 * @name        : uniform name
 */
void __shader_reserve_uniform(struct shader *p, i16 index, u8 type, char *name)
{
        if(index >= p->tracks->len) {
                while(index >= p->tracks->len) {
                        struct shader_uniform_track *t  = shader_uniform_track_alloc();
                        t->pipeline                     = p;
                        t->index                        = p->tracks->len;
                        if(index == t->index) {
                                t->type = type;
                                string_cat(t->name, name, strlen(name));
                        }
                        array_push(p->tracks, &t);
                        array_push(p->flags, &(i16){0});
                }
        } else {
                struct shader_uniform_track *t = array_get(p->tracks, struct shader_uniform_track *, index);
                t->type = type;
                t->name->len = 0;
                string_cat(t->name, name, strlen(name));
        }
}

/*
 * set current active pipeline
 */
void shader_use(struct shader *p)
{
        if(p != current_shader) {
                current_shader = p;
                glUseProgram(p->id);
        }
}

/*
 * update uniform datas
 * @p           : pipeline to manipulate
 * @frame       : current application frame
 */
void shader_update_uniform(struct shader *p, u8 frame)
{
        if(!p->pendings->len || !p->update[frame]) return;

        int i;
        for_i(i, BUFFERS) p->update[i] = 0;

        /* i don't know if it has to link shader before changing uniform */
        shader_use(p);
        i16 *pid;

        array_for_each(pid, p->pendings) {
                array_set(p->flags, *pid, &(i16){0});
                struct shader_uniform_track *t = array_get(p->tracks,
                        struct shader_uniform_track *, *pid);
                if(t->uniform) {
                        if(t->id == -1) {
                                t->id = glGetUniformLocation(p->id, t->name->ptr);
                        }
                        if(t->id >= 0) {
                                switch(t->type) {
                                        case UNIFORM_F1:
                                                glUniform1fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / sizeof(GLfloat)),
                                                        (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_F2:
                                                glUniform2fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (2 * sizeof(GLfloat))),
                                                        (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_F3:
                                                glUniform3fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (3 * sizeof(GLfloat))),
                                                        (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_F4:
                                                glUniform4fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (4 * sizeof(GLfloat))),
                                                        (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_I1:
                                                glUniform1iv(t->id,
                                                        (GLsizei)(t->uniform->data->len / sizeof(GLint)),
                                                        (GLint *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_I2:
                                                glUniform2iv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (2 * sizeof(GLint))),
                                                        (GLint *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_I3:
                                                glUniform3iv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (3 * sizeof(GLint))),
                                                        (GLint *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_I4:
                                                glUniform4iv(t->id,
                                                        (GLsizei)(t->uniform->data->len / (4 * sizeof(GLint))),
                                                        (GLint *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_M2:
                                                glUniformMatrix2fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / sizeof(union mat2)),
                                                        GL_FALSE, (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_M3:
                                                glUniformMatrix3fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / sizeof(union mat3)),
                                                        GL_FALSE, (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        case UNIFORM_M4:
                                                glUniformMatrix4fv(t->id,
                                                        (GLsizei)(t->uniform->data->len / sizeof(union mat4)),
                                                        GL_FALSE, (GLfloat *)t->uniform->data->ptr);
                                                break;

                                        default:
                                                break;
                                }
                        }
                }
        }
        array_force_len(p->pendings, 0);
}

void shader_setup_group(struct shader *p, struct device_buffer_group *g)
{
        glBindVertexArray(g->id);
        i16 i;
        for_i(i, p->descriptor->buffers->len) {
                struct shader_buffer_descriptor *sbd = array_get(p->descriptor->buffers,
                        struct shader_buffer_descriptor *, i);
                struct device_buffer *db = array_get(g->buffers, struct device_buffer *, i);
                glBindBuffer(device_buffer_target(db), db->id);
                struct shader_attribute_descriptor **sad;
                array_for_each(sad, sbd->attributes) {
                        GLint attr = glGetAttribLocation(p->id, (*sad)->name->ptr);
                        glEnableVertexAttribArray(attr);
                        i16 k;
                        switch((*sad)->type) {
                          case ATTRIBUTE_FLOAT:
                                glVertexAttribPointer(attr, 1, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                break;
                          case ATTRIBUTE_VEC2:
                                glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                break;
                          case ATTRIBUTE_VEC3:
                                glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                break;
                          case ATTRIBUTE_VEC4:
                                glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                break;
                          case ATTRIBUTE_MAT4:
                                glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                for_i_from(k, 1, 4) {
                                        attr++;
                                        glEnableVertexAttribArray(attr);
                                        glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                                sbd->vertex_size, (void*)(((*sad)->offset + k * 4) * sizeof(GLfloat)));
                                        if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                }
                                break;
                          case ATTRIBUTE_MAT3:
                                glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                for_i_from(k, 1, 3) {
                                        attr++;
                                        glEnableVertexAttribArray(attr);
                                        glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                                sbd->vertex_size, (void*)(((*sad)->offset + k * 4) * sizeof(GLfloat)));
                                        if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                }
                                break;
                          case ATTRIBUTE_MAT2:
                                glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                        sbd->vertex_size, (void*)((*sad)->offset * sizeof(GLfloat)));
                                if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                for_i_from(k, 1, 2) {
                                        attr++;
                                        glEnableVertexAttribArray(attr);
                                        glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE,
                                                sbd->vertex_size, (void*)(((*sad)->offset + k * 4) * sizeof(GLfloat)));
                                        if(sbd->instanced) glVertexAttribDivisor(attr, 1);
                                }
                                break;
                        }
                }
        }
        glBindVertexArray(0);
}

#endif
