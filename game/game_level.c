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
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/game/path.h>
#include <cherry/game/deliver.h>
#include <cherry/game/gem.h>
#include <cherry/xml/xml.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>

void game_parse_level(struct game *p, char *file)
{
        struct xml_element *xml = xml_parse(file);
        /*
         * parse path point
         */
        i16 i, j;
        for_i(i, 9) {
                for_i(j, 9) {
                        p->path_point_touches[i][j] = path_point_alloc();
                        list_add_tail(&p->path_point_touches[i][j]->life_head, &p->path_point_list);
                        p->path_point_touches[i][j]->position = (union vec3){(j - 4) * 200, (4 - i) * 200, 0};
                }
        }
        for_i(i, 9) {
                for_i(j, 9) {
                        struct path_point *pp = p->path_point_touches[i][j];
                        int ids[8][3] = {
                                {i - 1, j - 1, 0},
                                {i - 1, j    , 1},
                                {i - 1, j + 1, 2},
                                {i    , j + 1, 3},
                                {i + 1, j + 1, 4},
                                {i + 1, j    , 5},
                                {i + 1, j - 1, 6},
                                {i    , j - 1, 7}
                        };
                        i16 k;
                        for_i(k, 8) {
                                if(ids[k][0] >= 0 && ids[k][0] < 9 && ids[k][1] >= 0 && ids[k][1] < 9) {
                                        struct path_point *neighbor = p->path_point_touches[ids[k][0]][ids[k][1]];
                                        path_point_add_neighbor(pp, neighbor, ids[k][2]);
                                }
                        }
                }
        }
        /*
         * parse deliver lists
         */
        struct xml_element *delivers    = xml_find(xml, "delivers", 0);
        struct xml_element *types       = xml_find(delivers, "types", 0);
        struct map *map_type            = map_alloc(sizeof(int));
        struct list_head *head;
        list_for_each(head, &types->children) {
                struct xml_element *type        = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                struct xml_attribute *attr_type = xml_find_attribute(type, "type");
                struct xml_attribute *attr_id   = xml_find_attribute(type, "id");
                if(strcmp(attr_type->value->ptr, "path_point") == 0) {
                        int t                   = ELEMENT_DELIVER_PATH_POINT;
                        map_set(map_type, attr_id->value->ptr, attr_id->value->len, &t);
                }else if(strcmp(attr_type->value->ptr, "gate") == 0) {
                        int t                   = ELEMENT_DELIVER_GATE;
                        map_set(map_type, attr_id->value->ptr, attr_id->value->len, &t);
                }else if(strcmp(attr_type->value->ptr, "pool") == 0) {
                        int t                   = ELEMENT_DELIVER_POOL;
                        map_set(map_type, attr_id->value->ptr, attr_id->value->len, &t);
                }
        }
        struct xml_element *board       = xml_find(delivers, "board", 0);
        struct array *deliver_array     = array_alloc(sizeof(struct element_deliver *), ORDERED);
        array_reserve(deliver_array, 81);
        int index = 0;
        list_for_each(head, &board->children) {
                struct xml_element *d           = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                struct xml_attribute *attr_type = xml_find_attribute(d, "t");
                int dtype                       = map_get(map_type, int, attr_type->value->ptr, attr_type->value->len);
                struct element_deliver *ed = NULL;
                switch (dtype) {
                        case ELEMENT_DELIVER_PATH_POINT: {
                                /*
                                 * currently first top row is pool
                                 */
                                int row = (index - 9) / 9;
                                int col = (index - 9) % 9;
                                ed = element_deliver_alloc_path_point(p->path_point_touches[row][col]);
                        }
                                break;
                        case ELEMENT_DELIVER_POOL: {
                                if(index < 9) {
                                        ed = element_deliver_alloc_gem_list(&p->element_pool_list, (union vec4){(index - 4) * 200, 7 * 200, 0, 0});
                                } else {
                                        int row = (index - 9) / 9;
                                        int col = (index - 9) % 9;
                                        ed = element_deliver_alloc_gem_list(&p->element_pool_list, (union vec4){(col - 4) * 200, (4 - row) * 200, 0, 0});
                                }
                        }
                                break;
                }
                if(ed) list_add_tail(&ed->life_head, &p->deliver_list);
                array_push(deliver_array, &ed);
                index++;
        }

        struct xml_element *links = xml_find(delivers, "links", 0);
        index = 0;
        list_for_each(head, &links->children) {
                struct xml_element *link        = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                int row = index / 9;
                int col = index % 9;
                struct path_point *pp           = p->path_point_touches[row][col];
                struct list_head *to_head;
                list_for_each(to_head, &link->children) {
                        struct xml_element *to          = (struct xml_element *)
                                ((void *)to_head - offsetof(struct xml_element, element_head));
                        struct xml_attribute *a         = xml_find_attribute(to, "id");
                        int id                          = atoi(a->value->ptr);
                        struct element_deliver *d       = array_get(deliver_array, struct element_deliver *, id);
                        if(d) {
                                array_push(pp->delivers, &d);
                        }
                }
                index++;
        }

        map_free(map_type);
        array_free(deliver_array);
        xml_free(xml);
}
