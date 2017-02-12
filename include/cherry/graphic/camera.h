#ifndef __CHERRY_GRAPHIC_CAMERA_H__
#define __CHERRY_GRAPHIC_CAMERA_H__

#include <cherry/graphic/types.h>

struct camera *camera_alloc(union mat4 lookat);

void camera_free(struct camera *p);

void camera_move_around(struct camera *p, union vec3 offset);

void camera_move_forward(struct camera *p, float length);

void camera_rotate_self(struct camera *p, union vec4 quat);

void camera_rotate_around(struct camera *p, union vec4 quat);

union vec3 camera_get_position(struct camera *p);

union vec3 camera_get_direction(struct camera *p);

#endif
