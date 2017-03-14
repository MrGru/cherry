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
#include <cherry/graphic/node/action.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>

static inline void __action_reset(struct action *p)
{
        p->finish               = 0;
        p->remain               = p->duration;
        p->last_amount_offset   = (union vec4){0, 0, 0, 0};
        struct list_head *head;
        list_for_each(head, &p->children) {
                struct action *child = (struct action *)
                        ((void *)head - offsetof(struct action, head));
                __action_reset(child);
        }
}

static struct action *__action_alloc_sequence()
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_SEQUENCE;
        p->finish               = 0;
        p->repeat               = 0;
        p->action_type          = ACTION_OTHER;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

static struct action *__action_alloc_parallel()
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_PARALLEL;
        p->finish               = 0;
        p->repeat               = 0;
        p->action_type          = ACTION_OTHER;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}


struct action *action_alloc(u64 action_type, union vec4 *target, union vec4 offset, float duration, u8 type, i16 repeat)
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = type;
        p->target               = target;
        p->offset               = offset;
        p->last_amount_offset   = (union vec4){0, 0, 0, 0};
        p->duration             = duration;
        p->remain               = duration;
        p->finish               = 0;
        p->repeat               = repeat;
        p->action_type          = action_type;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct action *action_alloc_force(u64 action_type, union vec4 *target, union vec4 destination)
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_FORCE;
        p->target               = target;
        p->destination          = destination;
        p->finish               = 0;
        p->repeat               = 0;
        p->action_type          = action_type;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct action *action_alloc_gravity(u64 action_type, union vec4 *target, float velocity, float max_velocity, float accelerate, ...)
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_GRAVITY;
        p->target               = target;
        p->velocity             = velocity;
        p->max_velocity         = max_velocity;
        p->accelerate           = accelerate;
        p->finish               = 0;
        p->repeat               = 0;
        p->destinations         = array_alloc(sizeof(union vec4), ORDERED);
        p->index                = 0;
        p->action_type          = action_type;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        va_list parg;
        va_start(parg, accelerate);
        union vec4 *v = va_arg(parg, union vec4 *);
        while(v) {
                array_push(p->destinations, v);
                v = va_arg(parg, union vec4 *);
        }
        va_end(parg);

        return p;
}

struct action *action_alloc_delay(union vec4 *target, float duration)
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_DELAY;
        p->target               = target;
        p->delay                = duration;
        p->finish               = 0;
        p->repeat               = 0;
        p->action_type          = ACTION_OTHER;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct action *action_alloc_callback(union vec4 *target, void(*callback)(void *), void *callback_data)
{
        struct action *p        = smalloc(sizeof(struct action));
        p->ease_type            = EASE_CALLBACK;
        p->target               = target;
        p->callback             = callback;
        p->callback_data        = callback_data;
        p->finish               = 0;
        p->repeat               = 0;
        p->action_type          = ACTION_OTHER;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

void action_free(struct action *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct action *child = (struct action *)
                        ((void *)head - offsetof(struct action, head));
                action_free(child);
        }
        list_del(&p->head);
        list_del(&p->user_head);
        if(p->ease_type == EASE_GRAVITY) {
                array_free(p->destinations);
        }
        sfree(p);
}

static inline void __action_update_target(struct action *p, float percent)
{
        *p->target              = vec4_sub(*p->target, p->last_amount_offset);
        p->last_amount_offset   = vec4_mul_scalar(p->offset, percent);
        *p->target              = vec4_add(*p->target, p->last_amount_offset);
        if(p->remain <= EPSILON) {
                p->finish = 1;
                p->remain = 0.0f;
        }
}

static void __action_ease_linear(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * (2 - t);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * 0.5f;
                } else {
                        t = t - 1;
                        t = -0.5f * (t * (t - 2) - 1);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (t * t * t + 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * 0.5;
                } else {
                        t = t - 2;
                        t = (t * t * t + 2) / 2;
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = -(t * t * t * t - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = -0.5f * (t * t * t * t - 2);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (t * t * t * t * t + 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = (t * t * t * t * t + 2) * 0.5f;
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -cos(t * M_PI * 0.5f) + 1;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = sin(t * M_PI * 0.5f);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -0.5f * (cos(t * M_PI) - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = (float)pow(2.0, 10.0 * (t - 1));
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -(float)pow(2.0, -10.0 * t) + 1;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = 0.5f * (float)pow(2.0, 10.0 * (t - 1));
                } else {
                        t = t - 1;
                        t = 0.5f * (-(float)pow(2.0, -10.0 * t) + 2);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_in(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -((float)sqrt(1.0 - t * t) - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (float)sqrt(1.0 - t * t);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_in_out(struct action *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = -0.5f * ((float)sqrt(1 - t * t) - 1);
                } else {
                        t = t - 2;
                        t = 0.5f * ((float)sqrt(t - t * t) + 1);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_gravity(struct action *p, float delta)
{
        if(p->destinations->len == 0) {
                p->finish = 1;
                goto end;
        }
        union vec4 destination          = array_get(p->destinations, union vec4, p->index);
        union vec4 s                    = *p->target;

        if(smemcmp(&destination, &s, sizeof(union vec4)) == 0) goto finish;

        union vec4 direction            = vec4_normalize(vec4_sub(destination, s));

        s                               = vec4_add(s, vec4_mul_scalar(direction, p->velocity * delta));
        s                               = vec4_add(s, vec4_mul_scalar(direction, p->accelerate * delta * delta * 0.5f));

        *p->target                      = s;
        p->velocity                     = p->velocity + p->accelerate * delta;

        if(p->velocity > p->max_velocity) {
                p->velocity = p->max_velocity;
        }

        if(smemcmp(&destination, &s, sizeof(union vec4)) == 0) goto finish;

        union vec4 new_direction        = vec4_normalize(vec4_sub(destination, s));

        float c                         = vec4_length(vec4_add(direction, new_direction));

        if(c > 1) goto end;

finish:;
        p->index++;
        if(p->index == p->destinations->len) {
                *p->target              = destination;
                p->finish               = 1;
        }
end:;
}

static void __action_ease_force(struct action *p)
{
        *p->target = p->destination;
        p->finish = 1;
}

static void __action_ease_delay(struct action *p, float delta)
{
        p->delay -= delta;
        if(p->delay <= 0) {
                p->finish = 1;
        }
}

static void __action_ease_callback(struct action *p, float delta)
{
        if(p->callback) p->callback(p->callback_data);
        p->finish = 1;
}

u8 action_update(struct action *p, float delta, u64 *flag)
{
        u8 finish = 1;
        if( ! p->finish) {
                *flag |= p->action_type;
                switch (p->ease_type) {
                        case EASE_LINEAR:
                                __action_ease_linear(p, delta);
                                break;

                        case EASE_QUADRATIC_IN:
                                __action_ease_quadratic_in(p, delta);
                                break;

                        case EASE_QUADRATIC_OUT:
                                __action_ease_quadratic_out(p, delta);
                                break;

                        case EASE_QUADRATIC_IN_OUT:
                                __action_ease_quadratic_in_out(p, delta);
                                break;

                        case EASE_CUBIC_IN:
                                __action_ease_cubic_in(p, delta);
                                break;

                        case EASE_CUBIC_OUT:
                                __action_ease_cubic_out(p, delta);
                                break;

                        case EASE_CUBIC_IN_OUT:
                                __action_ease_cubic_in_out(p, delta);
                                break;

                        case EASE_QUARTIC_IN:
                                __action_ease_quartic_in(p, delta);
                                break;

                        case EASE_QUARTIC_OUT:
                                __action_ease_quartic_out(p, delta);
                                break;

                        case EASE_QUARTIC_IN_OUT:
                                __action_ease_quartic_in_out(p, delta);
                                break;

                        case EASE_QUINTIC_IN:
                                __action_ease_quintic_in(p, delta);
                                break;

                        case EASE_QUINTIC_OUT:
                                __action_ease_quintic_out(p, delta);
                                break;

                        case EASE_QUINTIC_IN_OUT:
                                __action_ease_quintic_in_out(p, delta);
                                break;

                        case EASE_SINUSOIDAL_IN:
                                __action_ease_sinusoidal_in(p, delta);
                                break;

                        case EASE_SINUSOIDAL_OUT:
                                __action_ease_sinusoidal_out(p, delta);
                                break;

                        case EASE_SINUSOIDAL_IN_OUT:
                                __action_ease_sinusoidal_in_out(p, delta);
                                break;

                        case EASE_EXPONENTIAL_IN:
                                __action_ease_exponential_in(p, delta);
                                break;

                        case EASE_EXPONENTIAL_OUT:
                                __action_ease_exponential_out(p, delta);
                                break;

                        case EASE_EXPONENTIAL_IN_OUT:
                                __action_ease_exponential_in_out(p, delta);
                                break;

                        case EASE_CIRCULAR_IN:
                                __action_ease_circular_in(p, delta);
                                break;

                        case EASE_CIRCULAR_OUT:
                                __action_ease_circular_out(p, delta);
                                break;

                        case EASE_CIRCULAR_IN_OUT:
                                __action_ease_circular_in_out(p, delta);
                                break;

                        case EASE_GRAVITY:
                                __action_ease_gravity(p, delta);
                                break;

                        case EASE_SEQUENCE:
                                {
                                        struct list_head *head;
                                        p->finish = 1;
                                        list_for_each(head, &p->children) {
                                                struct action *child = (struct action *)
                                                        ((void *)head - offsetof(struct action, head));
                                                if(child->finish) {
                                                        continue;
                                                } else {
                                                        u8 child_finish = action_update(child, delta, flag);
                                                        if(child_finish && head == p->children.prev) {
                                                                p->finish = 1;
                                                        } else {
                                                                p->finish = 0;
                                                        }
                                                        break;
                                                }
                                        }
                                }
                                break;

                        case EASE_PARALLEL:
                                {
                                        struct list_head *head;
                                        p->finish = 1;
                                        list_for_each(head, &p->children) {
                                                struct action *child = (struct action *)
                                                        ((void *)head - offsetof(struct action, head));
                                                u8 child_finish = action_update(child, delta, flag);
                                                p->finish &= child_finish;
                                        }
                                }
                                break;

                        case EASE_FORCE:
                                __action_ease_force(p);
                                break;
                        case EASE_DELAY:
                                __action_ease_delay(p, delta);
                                break;

                        case EASE_CALLBACK:
                                __action_ease_callback(p, delta);
                                break;

                        default:
                                p->finish = 1;
                                break;
                }
        }
        finish &= p->finish;
        if(finish) {
                if(p->repeat == -1) {
                        __action_reset(p);
                        finish = 0;
                } else if(p->repeat > 0) {
                        p->repeat--;
                        __action_reset(p);
                        finish = 0;
                }
        }
        return finish;
}

struct action *action_sequence(struct action *p, ...)
{
        struct action *se = __action_alloc_sequence();
        va_list parg;
        va_start(parg, p);
        struct action *child    = p;
        while(child) {
                list_add_tail(&child->head, &se->children);
                child = va_arg(parg, struct action *);
        }
        va_end(parg);
        return se;
}

struct action *action_parallel(struct action *p, ...)
{
        struct action *se = __action_alloc_parallel();
        va_list parg;
        va_start(parg, p);
        struct action *child    = p;
        while(child) {
                list_add_tail(&child->head, &se->children);
                child = va_arg(parg, struct action *);
        }
        va_end(parg);
        return se;
}

/*
 * action manager implementation
 */
struct action_manager *action_manager_alloc()
{
        struct action_manager *p = smalloc(sizeof(struct action_manager));
        INIT_LIST_HEAD(&p->keys);
        return p;
}

void action_manager_update(struct action_manager *p, float delta)
{
        struct list_head *head, *next;
        list_for_each_safe(head, next, &p->keys) {
                struct action_key *key = (struct action_key *)
                        ((void *)head - offsetof(struct action_key, key_head));
                u8 all_finish   = 1;
                u64 flag        = (u64)ACTION_OTHER;
                struct list_head *action_head, *action_head_next;
                list_for_each_safe(action_head, action_head_next, &key->actions) {
                        struct action *a = (struct action *)
                                ((void *)action_head - offsetof(struct action, head));
                        u8 finish = action_update(a, delta, &flag);
                        if(finish) {
                                action_free(a);
                        }
                        all_finish &= finish;
                }
                if(flag & ACTION_TRANSFORM) {
                        branch_transform_shake(key->transform);
                } else if(flag & ACTION_BRIGHT) {
                        twig_bright_update(key->bright, key->bright->bright);
                }
                if(all_finish) {
                        list_del_init(&key->key_head);
                }
        }
}

static inline void __action_manager_clear(struct action_manager *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->keys) {
                struct action_key *key = (struct action_key *)
                        ((void *)head - offsetof(struct action_key, key_head));
                action_key_clear(key);
        }
}

void action_manager_free(struct action_manager *p)
{
        __action_manager_clear(p);
        sfree(p);
}

void action_manager_add_key(struct action_manager *p, struct action_key * k)
{
        list_del(&k->key_head);
        list_add_tail(&k->key_head, &p->keys);
}

void action_key_add_action(struct action_key *p, struct action * a)
{
        list_del(&a->head);
        list_add_tail(&a->head, &p->actions);
}

void action_key_init(struct action_key *p)
{
        INIT_LIST_HEAD(&p->key_head);
        INIT_LIST_HEAD(&p->actions);
        p->transform    = NULL;
        p->bright       = NULL;
}

void action_key_clear(struct action_key *key)
{
        struct list_head *action_head;
        list_while_not_singular(action_head, &key->actions) {
                struct action *a = (struct action *)
                        ((void *)action_head - offsetof(struct action, head));
                action_free(a);
        }
        list_del_init(&key->key_head);
}
