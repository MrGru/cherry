#include <cherry/graphic/camera.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>

static inline void camera_update(struct camera *p)
{
        p->position_update = 1;
        shader_uniform_update(p->view_uniform, p->view.m, sizeof(p->view));
        union vec3 pos = camera_get_position(p);
        shader_uniform_update(p->position_uniform, pos.v, sizeof(pos));
}

struct camera *camera_alloc(union mat4 lookat)
{
        struct camera *p        = smalloc(sizeof(struct camera));
        p->view                 = lookat;
        p->position_update      = 1;
        p->view_uniform         = shader_uniform_alloc();
        p->view_uniform->ref++;
        p->position_uniform     = shader_uniform_alloc();
        p->position_uniform->ref++;
        camera_update(p);
        return p;
}

void camera_free(struct camera *p)
{
        shader_uniform_free(p->view_uniform);
        shader_uniform_free(p->position_uniform);
        sfree(p);
}

void camera_move_around(struct camera *p, union vec3 offset)
{
        p->view = mat4_translate(p->view, vec3_neg(offset));
        camera_update(p);
}

void camera_move_forward(struct camera *p, float length)
{
        p->view = mat4_translate(p->view, vec3_neg(vec3_mul_scalar(camera_get_direction(p), length)));
        camera_update(p);
}

void camera_rotate_self(struct camera *p, union vec4 quat)
{
        p->view = mat4_mul(mat4_from_quat(quat), p->view);
        camera_update(p);
}

void camera_rotate_around(struct camera *p, union vec4 quat)
{
        p->view = mat4_mul(p->view, mat4_from_quat(quat));
        camera_update(p);
}

union vec3 camera_get_position(struct camera *p)
{
        if(p->position_update) {
                p->position_update = 0;
                union mat4 inv = mat4_invert(p->view);
                p->position = vec3((float[3]){inv.m[12], inv.m[13], inv.m[14]});
        }
        return p->position;
}

union vec3 camera_get_direction(struct camera *p)
{
        return vec3_normalize(vec3((float[3]){-p->view.m[2], -p->view.m[6], -p->view.m[10]}));
}
