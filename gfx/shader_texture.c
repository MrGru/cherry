#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/string.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/device_buffer.h>

static struct shader *instance = NULL;

static void clear()
{
        if(instance) {
                shader_free(instance);
                instance = NULL;
        }
}

#if GFX == OGL

static inline struct string *get_vert()
{
        return file_read_string("res/shaders/shader_texture.vert");
}

static inline struct string *get_frag()
{
        return file_read_string("res/shaders/shader_texture.frag");
}

#elif GFX == MTL



#endif

struct shader *shader_texture_alloc()
{
        if(!instance) {
                cache_add(clear);
        }
        struct shader_descriptor *des = shader_descriptor_alloc();
        /* vertex */
        struct shader_buffer_descriptor *sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 0, 0);
        struct shader_attribute_descriptor *sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, "position");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /*z*/
        sbd = shader_buffer_descriptor_alloc(sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "z");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* transform */
        sbd = shader_buffer_descriptor_alloc(16 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_MAT4, 0, "transform");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* color */
        sbd = shader_buffer_descriptor_alloc(4 * sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC4, 0, "color");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* texid */
        sbd = shader_buffer_descriptor_alloc(sizeof(float), 1, 1);
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_FLOAT, 0, "texid");
        array_push(sbd->attributes, &sad);
        array_push(des->buffers, &sbd);
        /* texcoords */
#define ADD_TEXCOORDS(name)                                                     \
        sbd = shader_buffer_descriptor_alloc(2 * sizeof(float), 1, 1);          \
        sad = shader_attribute_descriptor_alloc(ATTRIBUTE_VEC2, 0, name);       \
        array_push(sbd->attributes, &sad);                                      \
        array_push(des->buffers, &sbd);

        ADD_TEXCOORDS("texcoord_1")
        ADD_TEXCOORDS("texcoord_2")
        ADD_TEXCOORDS("texcoord_3")
        ADD_TEXCOORDS("texcoord_4")
        ADD_TEXCOORDS("texcoord_5")
        ADD_TEXCOORDS("texcoord_6")

#undef ADD_TEXCOORDS

        /* vert */
        struct string *vert = get_vert();
        /* frag */
        struct string *frag = get_frag();
        instance = shader_alloc(vert->ptr, frag->ptr, des);
        string_free(vert);
        string_free(frag);

        shader_reserve_uniform(instance, SHADER_TEXTURE_PROJECT, UNIFORM_M4,
                "project", offsetof(struct shader_texture_uniform, project));
        shader_reserve_uniform(instance, SHADER_TEXTURE_VIEW, UNIFORM_M4,
                "view", offsetof(struct shader_texture_uniform, view));

#if GFX == MTL
        u8 i;
        struct shader_texture_uniform u;
        for_i(i, BUFFERS) {
                instance->uniforms[i] = device_buffer_alloc();
                device_buffer_fill(instance->uniforms[i], &u, sizeof(u));
        }
#endif

#if GFX == OGL
        u8 i;
        for_i_from(i, SHADER_TEXTURE_IMAGE_0, SHADER_TEXTURE_IMAGE_N) {
                struct string *name = string_alloc(10);
                string_cat(name, "image[", sizeof("image[") - 1);
                string_cat_int(name, i - SHADER_TEXTURE_IMAGE_0);
                string_cat(name, "]", sizeof("]") - 1);
                shader_reserve_uniform(instance, i, UNIFORM_I1, name->ptr, 0);

                struct shader_uniform *u = shader_uniform_alloc();
                shader_set_uniform(instance, i, u);
                u->ref++;
                array_push(instance->texture_uniforms, &u);

                u32 id = 0;
                shader_uniform_update(u, &id, sizeof(id));

                string_free(name);
        }
#endif

        return instance;
}
