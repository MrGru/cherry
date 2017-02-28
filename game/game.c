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
#include <cherry/game/game.h>
#include <cherry/game/gem.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/render.h>
#include <cherry/stdio.h>
#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/graphic/shader/descriptor_collection.h>
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/shader.h>
#include <cherry/math/math.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>
#include <cherry/list.h>
#include <cherry/graphic/camera.h>
#include <cherry/graphic/dae/dae_mesh.h>
#include <cherry/graphic/light/light.h>
#include <cherry/graphic/node/action.h>
#include <cherry/xml/xml.h>
#include <cherry/game/element.h>
#include <cherry/game/deliver.h>
#include <cherry/game/path.h>
#include <cherry/game/gem.h>

/*
 * create game content
 */
static inline void __game_create_game_content(struct game *p, struct render_queue *queue)
{
        struct array *buffers[BUFFERS];
        u32 triangles_per_object = GAME_TRIANGLES_PER_OBJECT;
        u32 instances = 300;
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_triangle_alloc(BUFFER_DEVICE)
                });
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_transform_alloc(instances, BUFFER_PINNED)});
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_color_alloc(instances, BUFFER_DEVICE)
                });
                u8 j;
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_3d_vertex_alloc(instances * triangles_per_object, BUFFER_PINNED)
                        });
                }
                for_i(j, 3) {
                        array_push(buffers[i], &(struct device_buffer *){
                                buffer_3d_normal_alloc(instances * triangles_per_object, BUFFER_PINNED)
                        });
                }
                array_push(buffers[i], &(struct device_buffer *){
                        buffer_vertex_color_alloc(instances * triangles_per_object, BUFFER_PINNED)
                });
        }

        struct render_content *content = render_content_alloc(queue, buffers, 3, instances, triangles_per_object);
        content->depth_test = 1;
        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        p->game_content = content;
}

static void __game_setup_base(struct game *p)
{
        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        renderer_set_color(r, &(union vec4){0, 0, 0, 1});
        list_add_tail(&r->chain_head, &p->renderer_list);

        struct render_stage *stage = render_stage_alloc(r);

        struct shader *s3d_color = shader_3d_color_alloc(0, 1, 0);
        s3d_color->descriptor = descriptor_3d_color_get(GAME_TRIANGLES_PER_OBJECT);
        struct render_queue *s3d_queue = render_queue_alloc(&stage->content_queue_list, s3d_color);
        __game_create_game_content(p, s3d_queue);
        p->game_projection_uniform = shader_uniform_alloc();
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_PROJECT, p->game_projection_uniform);

        struct point_light *pl = point_light_alloc();
	point_light_set_position(pl, vec3((float[3]){0, 200, 1000}));
	point_light_set_constant(pl, 1.0f);
	point_light_set_linear(pl, 0.00014f);
	point_light_set_quadratic(pl, 0.00000001f);
	point_light_set_ambient(pl, vec3((float[3]){0.5, 0.5, 0.5}));
	point_light_set_diffuse(pl, vec3((float[3]){0.5, 0.5, 0.5}));
	point_light_set_specular(pl, vec3((float[3]){0.2, 0.2, 0.2}));

        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_POSITION, pl->u_position);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_CONSTANT, pl->u_constant);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_LINEAR, pl->u_linear);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_QUADRATIC, pl->u_quadratic);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_AMBIENT, pl->u_ambient);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_DIFFUSE, pl->u_diffuse);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_POINT_LIGHT_0_SPECULAR, pl->u_specular);

        p->world_light = pl;

        p->game_cam = camera_alloc(mat4_new_look_at(
                0, 0, 4100,
                0, 0, 0,
                0, 1, 0
        ));
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_VIEW, p->game_cam->view_uniform);
        shader_set_uniform(s3d_color, SHADER_3D_COLOR_VIEW_POSITION, p->game_cam->position_uniform);
        union mat4 project = mat4_new_perspective(DEG_TO_RAD(45.0f), video_width * 1.0f / video_height, 1.0f, 10000.0f);
        shader_uniform_update(p->game_projection_uniform, project.m, sizeof(project));

        camera_rotate_around(p->game_cam, quat_angle_axis(DEG_TO_RAD(-15), (float[3]){1, 0, 0}));
}

void game_clear(struct game *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->node_3d_color_list) {
                struct node_3d_color *n = (struct node_3d_color *)
                        ((void *)head - offsetof(struct node_3d_color, life_head));
                node_3d_color_free(n);
        }
        list_while_not_singular(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_free(renderer);
        }
        list_while_not_singular(head, &p->path_point_list) {
                struct path_point *pp = (struct path_point *)
                        ((void *)head - offsetof(struct path_point, life_head));
                path_point_free(pp);
        }
        list_while_not_singular(head, &p->deliver_list) {
                struct element_deliver *ed = (struct element_deliver *)
                        ((void *)head - offsetof(struct element_deliver, life_head));
                element_deliver_free(ed);
        }

        if(p->n3d_update_queue) {
                branch_transform_queue_free(p->n3d_update_queue);
        }
        p->n3d_update_queue = branch_transform_queue_alloc();

        if(p->action_manager) {
                action_manager_free(p->action_manager);
        }
        p->action_manager = action_manager_alloc();
}


struct game *game_alloc()
{
        time_t t;
        /* Intializes random number generator */
        srand((unsigned) time(&t));
        int i, j;
        struct game *p  = smalloc(sizeof(struct game));
        INIT_LIST_HEAD(&p->renderer_list);
        INIT_LIST_HEAD(&p->node_3d_color_list);
        INIT_LIST_HEAD(&p->deliver_list);
        INIT_LIST_HEAD(&p->element_list);
        INIT_LIST_HEAD(&p->element_pool_list);
        INIT_LIST_HEAD(&p->path_point_list);
        INIT_LIST_HEAD(&p->element_update_pos_list);

        p->n3d_update_queue     = branch_transform_queue_alloc();
        p->frame                = 0;
        p->action_manager       = action_manager_alloc();
        p->can_draw             = BUFFERS;

        __game_setup_base(p);

        game_parse_level(p, "res/levels/level_1.xml");

        struct node_3d_color *n1 = game_empty_node_alloc(p);
        // union vec4 color[6] = {
        //         (union vec4){255 / 255.0f, 0 / 255.0f, 55 / 255.0f, 1}, //RED
        //         (union vec4){0 / 255.0f, 120 / 255.0f, 255 / 255.0f, 1}, //BLUE
        //         (union vec4){72 / 255.0f, 255 / 255.0f, 0 / 255.0f, 1}, //GREEN
        //         (union vec4){255 / 255.0f, 255 / 255.0f, 0 / 255.0f, 1}, //YELLOW
        //         (union vec4){182 / 255.0f, 0 / 255.0f, 255 / 255.0f, 1}, //PURPLE
        //         (union vec4){255 / 255.0f, 86 / 255.0f, 0 / 255.0f, 1}, //ORANGE
        // };
        union vec4 color[6] = {
                (union vec4){1, 1, 1, 1},
                (union vec4){1, 1, 1, 1},
                (union vec4){1, 1, 1, 1},
                (union vec4){1, 1, 1, 1},
                (union vec4){1, 1, 1, 1},
                (union vec4){1, 1, 1, 1}
        };

        int test_type = 6;
        int test[6] = {GEM_1_LV_1, GEM_2_LV_1,
                GEM_5_LV_1, GEM_6_LV_1, GEM_3_LV_1, GEM_4_LV_1};
        for_i(i, 9) {
                for_i(j, 9) {
                        struct gem *gem = gem_alloc(GEM_1_LV_1);
                        list_add_tail(&gem->elm.life_head, &p->element_list);
                        list_add_tail(&gem->elm.path_head, &p->element_pool_list);
                        int ct          = rand_ri(0, 6);
                        struct node_3d_color *node, *flipped_node;
                        int t = rand_ri(0, 1000);
                        i16 type = test[rand_ri(0, 5)];
                        {
                                struct node_3d_color *n2 = game_gem_alloc(p, gem_mesh_cache(type));
                                node_3d_color_add_node_3d_color(n1, n2);
                                node_3d_color_set_position(n2, (union vec3){(i - 4) * 200, (j - 4) * 200, 0});
                                node_3d_color_set_color(n2, color[ct]);
                                node = n2;
                        }
                        {
                                struct node_3d_color *n2 = game_gem_alloc(p, gem_mesh_cache(type));
                                node_3d_color_add_node_3d_color(n1, n2);
                                node_3d_color_set_position(n2, (union vec3){(i - 4) * 200, (j - 4) * 200, -200});
                                node_3d_color_set_color(n2, vec4_mul_scalar(color[ct], 1.0));
                                flipped_node = n2;
                        }
                        gem_set_node(gem, node, flipped_node);
                }
        }
        {
                struct node_3d_color *n2 = game_floor_node_alloc(p);
                node_3d_color_add_node_3d_color(n1, n2);
                node_3d_color_set_position(n2, (union vec3){0, 0, -100});
                node_3d_color_set_color(n2, (union vec4){0.0, 0.0, 0.0, 0.575});
        }

        // {
        //         struct dae_mesh *pipe = dae_mesh_alloc("res/models/cell_1.dae");
        //         for(i = -12; i < 15; i+= 3) {
        //                 struct node_3d_color *n2 = game_cell_alloc(p, pipe, i);
        //                 node_3d_color_add_node_3d_color(n1, n2);
        //                 node_3d_color_set_color(n2, (union vec4){1, 1, 1, 0.05});
        //         }
        //         dae_mesh_free(pipe);
        // }
        //
        // {
        //         struct dae_mesh *pipe = dae_mesh_alloc("res/models/plane_1.dae");
        //         for(i = -12; i < 15; i+= 3) {
        //                 struct node_3d_color *n2 = game_plane_alloc(p, pipe, i);
        //                 node_3d_color_add_node_3d_color(n1, n2);
        //                 node_3d_color_set_color(n2, (union vec4){1, 1, 1, 0.05});
        //         }
        //         dae_mesh_free(pipe);
        // }

        /* recalculate color tree */
        branch_color_traverse(node_3d_color_get_branch_color(n1), (union vec4){1, 1, 1, 1});

        back_i(i, 9) {
                for_i(j, 9) {
                        path_point_check_move(p->path_point_touches[i][j], p);
                }
        }
        return p;
}

static void game_update_element_pos(struct game *p)
{
        struct list_head *head, *next;
        i16 flag = 0;
        list_for_each_safe(head, next, &p->element_update_pos_list) {
                struct game_element *ge = (struct game_element *)
                        ((void *)head - offsetof(struct game_element, update_pos_head));
                game_element_update_pos(ge);
                flag = 1;
        }
        if(flag) p->can_draw = BUFFERS;
}

void game_update(struct game *p)
{
        // camera_rotate_around(p->game_cam, quat_angle_axis(DEG_TO_RAD(0.5), (float[3]){0, 0, 1}));
        // union vec4 r = *node_3d_color_get_rotation(test_node);
        // union vec4 q = quat_mul(quat_angle_axis(DEG_TO_RAD(1), (float[3]){1, 0, 0}), r);
        // node_3d_color_set_rotation(test_node, q);
        action_manager_update(p->action_manager, 1.0f / 60);
        game_update_element_pos(p);
        branch_transform_queue_traverse(p->n3d_update_queue);
}

void game_render(struct game *p)
{
        /* process all renderers */
        struct list_head *head;
        list_for_each(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_render(renderer, p->frame);
        }
        /* increase frame by 1 */
        p->frame++;
        p->frame %= BUFFERS;
        p->can_draw--;
}

static inline void __game_resize_game_content(struct game *p)
{
        union mat4 project = mat4_new_perspective(DEG_TO_RAD(45.0f), video_width * 1.0f / video_height, 1.0f, 10000.0f);
        shader_uniform_update(p->game_projection_uniform, project.m, sizeof(project));
}

void game_resize(struct game *p, int width, int height)
{
        video_width = width;
        video_height = height;

        __game_resize_game_content(p);
        p->can_draw = BUFFERS;
}

void game_free(struct game *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->node_3d_color_list) {
                struct node_3d_color *n = (struct node_3d_color *)
                        ((void *)head - offsetof(struct node_3d_color, life_head));
                node_3d_color_free(n);
        }
        list_while_not_singular(head, &p->renderer_list) {
                struct renderer *renderer = (struct renderer *)
                        ((void *)head - offsetof(struct renderer, chain_head));
                renderer_free(renderer);
        }
        list_while_not_singular(head, &p->path_point_list) {
                struct path_point *pp = (struct path_point *)
                        ((void *)head - offsetof(struct path_point, life_head));
                path_point_free(pp);
        }
        list_while_not_singular(head, &p->deliver_list) {
                struct element_deliver *ed = (struct element_deliver *)
                        ((void *)head - offsetof(struct element_deliver, life_head));
                element_deliver_free(ed);
        }
        list_while_not_singular(head, &p->element_list) {
                struct game_element *ge = (struct game_element *)
                        ((void *)head - offsetof(struct game_element, life_head));
                game_element_free(ge);
        }
        branch_transform_queue_free(p->n3d_update_queue);
        point_light_free(p->world_light);
        camera_free(p->game_cam);
        action_manager_free(p->action_manager);
        sfree(p);
}
