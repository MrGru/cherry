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
#include <cherry/game/path.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/game/deliver.h>
#include <cherry/graphic/node/action.h>
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/graphic/node/branch.h>

struct path_point *path_point_alloc()
{
        struct path_point *p    = smalloc(sizeof(struct path_point));
        u8 i;
        for_i(i, 8) {
                INIT_LIST_HEAD(&p->neighbor[i]);
                INIT_LIST_HEAD(&p->neighbor_head[i]);
        }
        INIT_LIST_HEAD(&p->dynamic_element);
        INIT_LIST_HEAD(&p->obstacle_lv1);
        INIT_LIST_HEAD(&p->obstacle_lv2);
        INIT_LIST_HEAD(&p->life_head);
        p->delivers             = array_alloc(sizeof(struct element_deliver *), ORDERED);
        p->position_expanded    = (union vec4){0 , 0, 0, 0};
        return p;
}

void path_point_free(struct path_point *p)
{
        list_del(&p->life_head);
        array_free(p->delivers);
        sfree(p);
}

void path_point_add_neighbor(struct path_point *p, struct path_point *n, u8 index)
{
        if(!list_singular(&p->neighbor[index])) {
                list_del_init(p->neighbor[index].next);
        }
        if(n) {
                u8 n_index = (index + 4) % 8;
                if(!list_singular(&n->neighbor_head[n_index])) {
                        list_del(&n->neighbor_head[n_index]);
                }
                list_add_tail(&n->neighbor_head[n_index], &p->neighbor[index]);
        }
}

static struct game_element *path_point_trace(struct path_point *p, struct list_head *trace_head)
{
        if(!list_singular(&p->dynamic_element)) {
                return (struct game_element *)((void*)p->dynamic_element.next - offsetof(struct game_element, path_head));
        } else {
                i16 i;
                for_i(i, p->delivers->len) {
                        struct element_deliver *deliver = array_get(p->delivers, struct element_deliver *, i);
                        list_add_tail(&deliver->trace_head, trace_head);
                        struct game_element *elm = NULL;
                        switch (deliver->type) {
                                case ELEMENT_DELIVER_PATH_POINT:
                                case ELEMENT_DELIVER_GATE:
                                        elm = path_point_trace(deliver->ppoint, trace_head);
                                        break;

                                case ELEMENT_DELIVER_POOL:
                                        if(!list_singular(deliver->element_list)) {
                                                elm = (struct game_element *)
                                                        ((void *)deliver->element_list->next - offsetof(struct game_element, path_head));
                                                list_del_init(&elm->path_head);
                                        }
                                        break;

                                default:
                                        break;
                        }
                        if(elm) {
                                return elm;
                        } else {
                                list_del(&deliver->trace_head);
                        }
                }
                return NULL;
        }
}

static inline void __move_gem(struct path_point *p, struct game *game, struct game_element *elm, struct list_head *list)
{
        struct gem *gem = (struct gem *)
                ((void *)elm - offsetof(struct gem, elm));
        /*
         * build move sequence
         */
        struct action *move_sequence    = action_sequence(NULL);
        struct branch_transform *branch = node_3d_color_get_branch_transform(gem->node);
        struct action *move_action      = action_alloc_gravity(&branch->position_expanded,
                300, 55000, 2000, NULL);
        list_add_tail(&move_action->head, &move_sequence->children);

        struct list_head *head, *next;
        float move_delay = 0;
        list_back_each_safe(head, next, list) {
                struct element_deliver *deliver = (struct element_deliver *)
                        ((void *)head - offsetof(struct element_deliver, trace_head));
                switch (deliver->type) {
                        case ELEMENT_DELIVER_PATH_POINT:
                                array_push(move_action->destinations, &deliver->ppoint->position_expanded);
                                break;

                        case ELEMENT_DELIVER_GATE:
                                array_push(move_action->destinations, &deliver->ppoint->position_expanded);
                                array_push(move_action->destinations, &deliver->gate_pos_expanded);
                                if(next) {
                                        struct element_deliver *next_deliver = (struct element_deliver *)
                                                ((void *)next - offsetof(struct element_deliver, trace_head));
                                        struct action *pin = action_alloc_force(&branch->position_expanded,
                                                next_deliver->position_expanded);
                                        list_add_tail(&pin->head, &move_sequence->children);
                                        move_action = action_alloc_gravity(&branch->position_expanded,
                                                300, 55000, 2000, NULL);
                                        list_add_tail(&move_action->head, &move_sequence->children);
                                }
                                break;

                        case ELEMENT_DELIVER_POOL:
                                {
                                        struct action *pin = action_alloc_force(&branch->position_expanded,
                                                deliver->position_expanded);
                                        list_add_tail(&pin->head, &move_sequence->children);
                                        struct action *delay = action_alloc_delay(&branch->position_expanded, deliver->delay);
                                        list_add_tail(&delay->head, &move_sequence->children);
                                        move_delay += deliver->delay;
                                        deliver->delay += rand_rf(0.3f, 0.5f);
                                        move_action = action_alloc_gravity(&branch->position_expanded,
                                                300, 55000, 2000, NULL);
                                        list_add_tail(&move_action->head, &move_sequence->children);
                                }
                                break;
                }
        }
        array_push(move_action->destinations, &p->position_expanded);
        /*
         * build rotate sequence
         */
        struct action *rotate_sequence  = action_sequence(NULL);
        struct rotation_vector *rv = rotation_vector_alloc();
        rv->rad_vec3 = (union vec4){0, 1, 0, 0};
        struct action *rotate_action    = action_alloc_gravity(&rv->rad_vec3,
                4, 16.5, 8, NULL);
        list_add_tail(&rotate_action->head, &rotate_sequence->children);
        union vec4 target = rv->rad_vec3;
        list_back_each_safe(head, next, list) {
                struct element_deliver *deliver = (struct element_deliver *)
                        ((void *)head - offsetof(struct element_deliver, trace_head));
                switch (deliver->type) {
                        case ELEMENT_DELIVER_PATH_POINT:
                                target.x += (DEG_TO_RAD(90));
                                array_push(rotate_action->destinations, &target);
                                break;

                        case ELEMENT_DELIVER_GATE:
                                target.x += (DEG_TO_RAD(90));
                                target.x += (DEG_TO_RAD(90));
                                array_push(rotate_action->destinations, &target);
                                break;

                        case ELEMENT_DELIVER_POOL:
                                target.x += (DEG_TO_RAD(90));
                                array_push(rotate_action->destinations, &target);
                                break;
                }
        }
        if(move_delay > 0) {
                target.x += (DEG_TO_RAD(90 * (int)(move_delay / 0.09f)));
                array_push(rotate_action->destinations, &target);
        }
        struct action *bonus = action_sequence(
                action_parallel(
                        action_alloc(&branch->position_expanded,
                                (union vec4){0, 5, 0, 0},
                                0.2f, EASE_QUADRATIC_IN_OUT, 0),
                        action_alloc(&rv->rad_vec3,
                                (union vec4){DEG_TO_RAD(-5), 0, 0, 0},
                                0.2f, EASE_QUADRATIC_IN_OUT, 0),
                        NULL
                ),
                action_parallel(
                        action_alloc(&branch->position_expanded,
                                (union vec4){0, -5, 0, 0},
                                0.2f, EASE_QUADRATIC_IN_OUT, 0),
                        action_alloc(&rv->rad_vec3,
                                (union vec4){DEG_TO_RAD(5), 0, 0, 0},
                                0.2f, EASE_QUADRATIC_IN_OUT, 0),
                        NULL
                ),
                NULL
        );
        list_add_tail(&bonus->head, &rotate_sequence->children);

        /*
         * combine and execute actions
         */
        struct action *gem_action = action_parallel(
                rotate_sequence,
                move_sequence,
                NULL
        );

        /*
         * link gem_action and rotation_vector so that branch transform can know
         * when rotate action is finished to reduce rotate calculations
         */
        list_add_tail(&gem_action->user_head, &rv->action);
        list_add_tail(&rv->head, &branch->anim_rotations);
        action_key_add_action(&gem->node_move_key, gem_action);
        action_manager_add_key(game->action_manager, &gem->node_move_key);
        if(list_singular(&gem->elm.update_pos_head)) {
                list_add_tail(&gem->elm.update_pos_head, &game->element_update_pos_list);
        }
}

void path_point_check_move(struct path_point *p, struct game *game)
{
        struct list_head        list;
        INIT_LIST_HEAD(&list);
        struct game_element *elm = path_point_trace(p, &list);
        if( ! list_singular(&list)) {
                /*
                 * detach element from current path point or pool
                 */
                list_del(&elm->path_head);
                /*
                 * attach element to path point
                 */
                list_add_tail(&elm->path_head, &p->dynamic_element);
                /*
                 * move gem to path point
                 */
                if(element_is_gem(elm)) {
                        __move_gem(p, game, elm, &list);
                }
        }
}
