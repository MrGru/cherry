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
        return file_read_string("res/shaders/shader_color.vert");
}

static inline struct string *get_frag()
{
        return file_read_string("res/shaders/shader_color.frag");
}

#elif GFX == MTL



#endif

struct shader *shader_color_alloc()
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

        /* vert */
        struct string *vert = get_vert();
        /* frag */
        struct string *frag = get_frag();
        instance = shader_alloc(vert->ptr, frag->ptr, des);
        string_free(vert);
        string_free(frag);

        shader_reserve_uniform(instance, SHADER_COLOR_PROJECT, UNIFORM_M4,
                "project", offsetof(struct shader_color_uniform, project));
        shader_reserve_uniform(instance, SHADER_COLOR_VIEW, UNIFORM_M4,
                "view", offsetof(struct shader_color_uniform, view));

#if GFX == MTL
        u8 i;
        struct shader_color_uniform scu;
        for_i(i, BUFFERS) {
                instance->uniforms[i] = device_buffer_alloc();
                device_buffer_fill(instance->uniforms[i], &scu, sizeof(scu));
        }
#endif

        return instance;
}
