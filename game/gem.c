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
#include <cherry/game/gem.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/graphic/node/action.h>
#include <cherry/graphic/node/node_3d_color.h>
#include <cherry/math/math.h>
#include <cherry/game/element.h>
#include <cherry/graphic/dae/dae_mesh.h>
#include <cherry/array.h>
#include <cherry/map.h>

static struct map *gem_cache = NULL;

static void gem_cache_clear()
{
        if(gem_cache) {
                map_deep_free(gem_cache, struct dae_mesh *, dae_mesh_free);
                gem_cache = NULL;
        }
}

struct dae_mesh *gem_mesh_cache(u16 type)
{
        if(!gem_cache) {
                gem_cache = map_alloc(sizeof(struct dae_mesh *));
                cache_add(gem_cache_clear);
        }
        struct dae_mesh *mesh = map_get(gem_cache, struct dae_mesh *, qpkey(type));
        if(mesh) goto finish;

        switch (type) {
                case GEM_1_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_2_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_3_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_4_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_5_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_6_LV_1:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;

                case GEM_1_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_2_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_3_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_4_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_5_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_6_LV_2:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;

                case GEM_1_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_2_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_3_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_4_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_5_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                case GEM_6_LV_3:
                        mesh = dae_mesh_alloc("res/models/gem_3.dae");
                        break;
                default:
                        break;
        }
        if(!mesh) goto finish;

        map_set(gem_cache, qpkey(type), &mesh);

finish:;
        return mesh;
}

struct gem *gem_alloc(u16 type)
{
        struct gem *p   = smalloc(sizeof(struct gem));
        game_element_init(&p->elm);
        p->elm.type     = type;
        p->node         = NULL;
        p->flipped_node = NULL;

        action_key_init(&p->node_move_key, NULL);
        action_key_init(&p->node_choice_key, NULL);
        action_key_init(&p->node_unchoice_key, NULL);
        action_key_init(&p->node_collected_key, NULL);
        return p;
}

void gem_free(struct gem *p)
{
        action_key_clear(&p->node_move_key);
        action_key_clear(&p->node_choice_key);
        action_key_clear(&p->node_unchoice_key);
        action_key_clear(&p->node_collected_key);

        list_del(&p->elm.update_pos_head);
        list_del(&p->elm.path_head);
        list_del(&p->elm.life_head);
        sfree(p);
}

void gem_update_node(struct gem *p)
{
        struct branch_transform *br     = node_3d_color_get_branch_transform(p->node);
        union vec3 pos                  = br->position;
        union vec3 scale                = br->scale;
        union vec4 quat                 = br->quat;

        pos.z                           = -pos.z - br->size.z * 2;
        scale.z                         = -scale.z;
        node_3d_color_set_position(p->flipped_node, pos);
        node_3d_color_set_scale(p->flipped_node, scale);
        node_3d_color_set_rotation(p->flipped_node, quat);

        if(list_singular(&p->node_move_key.key_head)
                && list_singular(&p->node_choice_key.key_head)
                && list_singular(&p->node_unchoice_key.key_head)
                && list_singular(&p->node_collected_key.key_head)) {
                        list_del_init(&p->elm.update_pos_head);
                }
}

void gem_set_node(struct gem *p, struct node_3d_color *node, struct node_3d_color *flipped_node)
{
        p->node = node;
        p->flipped_node = flipped_node;
        p->node_move_key.transform = node_3d_color_get_branch_transform(node);
        p->node_choice_key.transform = node_3d_color_get_branch_transform(node);
        p->node_unchoice_key.transform = node_3d_color_get_branch_transform(node);
        p->node_collected_key.transform = node_3d_color_get_branch_transform(node);
}
