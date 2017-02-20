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
#include <cherry/graphic/dae/dae_mesh.h>
#include <cherry/memory.h>
#include <cherry/bytes.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/xml/xml.h>
#include <cherry/math/math.h>
#include <cherry/list.h>

struct geo_mesh {
        struct string   *id;
        struct array    *positions;
        struct array    *normals;
        struct array    *colors;
        struct array    *uvs;
        struct array    *vcount;
        struct array    *p;
};

static struct geo_mesh *__geo_mesh_alloc()
{
        struct geo_mesh *p      = smalloc(sizeof(struct geo_mesh));
        p->id                   = string_alloc(0);
        p->positions            = array_alloc(sizeof(union vec3), ORDERED);
        p->normals              = array_alloc(sizeof(union vec3), ORDERED);
        p->uvs                  = array_alloc(sizeof(union vec2), ORDERED);
        p->colors               = array_alloc(sizeof(union vec3), ORDERED);
        p->vcount               = array_alloc(sizeof(int), ORDERED);
        p->p                    = array_alloc(sizeof(int), ORDERED);
        return p;
}

static void __geo_mesh_free(struct geo_mesh *p)
{
        string_free(p->id);
        array_free(p->positions);
        array_free(p->normals);
        array_free(p->uvs);
        array_free(p->colors);
        array_free(p->vcount);
        array_free(p->p);
        sfree(p);
}

static void __push_vec3_array(struct array *arr, struct string *text)
{
        char temp[64];
        union vec3 p;
        int j;
        int temp_index;
        int p_index;
        int size;
        for(j = 0, temp_index = 0, p_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        temp[temp_index] = '\0';
                        p.v[p_index] = atof(temp);
                        p_index++;
                        if(p_index == 3) {
                                array_push(arr, &p);
                                p_index = 0;
                        }
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
}

static void __push_vec2_array(struct array *arr, struct string *text)
{
        char temp[64];
        union vec2 p;
        int j;
        int temp_index;
        int p_index;
        int size;
        for(j = 0, temp_index = 0, p_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        temp[temp_index] = '\0';
                        p.v[p_index] = atof(temp);
                        p_index++;
                        if(p_index == 2) {
                                array_push(arr, &p);
                                p_index = 0;
                        }
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
}

static void __push_float_array(struct array *arr, struct string *text)
{
        char temp[64];
        float p;
        int j;
        int temp_index;
        int size;
        for(j = 0, temp_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        if(temp_index == 0) continue;
                        temp[temp_index] = '\0';
                        p= atof(temp);
                        array_push(arr, &p);
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
}

static void __push_int_array(struct array *arr, struct string *text)
{
        char temp[64];
        int p;
        int j;
        int temp_index;
        int size;
        for(j = 0, temp_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        if(temp_index == 0) continue;
                        temp[temp_index] = '\0';
                        p= atoi(temp);
                        array_push(arr, &p);
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
}

static union mat4 __convert_mat4(struct string *text)
{
        char temp[64];
        union mat4 ret;
        int j, temp_index, ret_index, size;

        for(j = 0, temp_index = 0, ret_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        if(temp_index == 0) continue;
                        temp[temp_index] = '\0';
                        ret.m[ret_index] = atof(temp);
                        ret_index++;
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
        return ret;
}

static void __push_mat4_array(struct array *arr, struct string *text)
{
        char temp[64];
        union mat4 ret;
        int j, temp_index, ret_index, size;

        for(j = 0, temp_index = 0, ret_index = 0, size = text->len; j <= size; j++) {
                temp[temp_index] = text->ptr[j];
                if(temp[temp_index] == ' ' || j == size) {
                        if(temp_index == 0) continue;
                        temp[temp_index] = '\0';
                        ret.m[ret_index] = atof(temp);
                        ret_index++;
                        if(ret_index == 16) {
                                array_push(arr, &ret);
                                ret_index = 0;
                        }
                        temp_index = 0;
                } else {
                        temp_index++;
                }
        }
}

static void __push_string_array(struct array *arr, struct string *text)
{
        int start = -1;
        int end = -2;
        int i;
        int size = text->len;
        for(i = 0; i <= size; i++) {
                if(text->ptr[i] == ' ' || i == size) {
                        if(end >= start) {
                                struct string* s = string_alloc(0);
                                string_cat(s, text->ptr + start, end - start + 1);
                                array_push(arr, &s);
                        }
                        start = -1;
                        end = -2;
                } else {
                        if(start == -1) start = i;
                        end = i;
                }
        }
}

static void __parse_geometry(struct array *result, struct xml_element *xml)
{
        struct xml_element *library_geometries = xml_find(xml, "library_geometries", 0);
        struct list_head *head;
        int count;
        list_for_each(head, &library_geometries->children) {
                struct xml_element *geometry    = (struct xml_element *)
                        ((void *)head - offsetof(struct xml_element, element_head));
                struct geo_mesh *mesh           = __geo_mesh_alloc();
                array_push(result, &mesh);
                string_cat_string(mesh->id, xml_find_attribute(geometry, "id")->value);

                struct xml_element* vertices = xml_find(geometry, "vertices", 0);
                struct xml_element* vertices_input = xml_find_deep(vertices, "input", "semantic", "POSITION");
                struct xml_element* polylist = xml_find(geometry, "polylist", 0);

                struct xml_element* input_vertex = xml_find_deep(polylist, "input", "semantic", "VERTEX");
                struct xml_element* input_normal = xml_find_deep(polylist, "input", "semantic", "NORMAL");
                struct xml_element* input_texcoord = xml_find_deep(polylist, "input", "semantic", "TEXCOORD");
                struct xml_element* input_color = xml_find_deep(polylist, "input", "semantic", "COLOR");
                struct xml_element* vcount = xml_find(polylist, "vcount", 0);
                struct xml_element* p = xml_find(polylist, "p", 0);

                struct xml_element* source_position = xml_find_deep(geometry, "source", "id",
                        xml_find_attribute(vertices_input, "source")->value->ptr+ 1);
                struct xml_element* source_position_float_array = xml_find(source_position, "float_array", 0);
                count = atoi(xml_find_attribute(source_position_float_array, "count")->value->ptr);
                array_reserve(mesh->positions, count / 3);
                __push_vec3_array(mesh->positions, source_position_float_array->value);

                struct xml_element* source_normal = xml_find_deep(geometry, "source", "id",
                        xml_find_attribute(input_normal, "source")->value->ptr+ 1);
                struct xml_element* source_normal_float_array = xml_find(source_normal, "float_array", 0);
                count = atoi(xml_find_attribute(source_normal_float_array, "count")->value->ptr);
                array_reserve(mesh->normals, count / 3);
                __push_vec3_array(mesh->normals, source_normal_float_array->value);

                if(input_texcoord) {
                struct xml_element* source_texcoord = xml_find_deep(geometry, "source", "id",
                        xml_find_attribute(input_texcoord, "source")->value->ptr+ 1);
                struct xml_element* source_texcoord_float_array = xml_find(source_texcoord, "float_array", 0);
                count = atoi(xml_find_attribute(source_texcoord_float_array, "count")->value->ptr);
                array_reserve(mesh->uvs, count / 2);
                __push_vec2_array(mesh->uvs, source_texcoord_float_array->value);
                }

                if(input_color) {
                struct xml_element* source_color = xml_find_deep(geometry, "source", "id",
                        xml_find_attribute(input_color, "source")->value->ptr+ 1);
                struct xml_element* source_color_float_array = xml_find(source_color, "float_array", 0);
                count = atoi(xml_find_attribute(source_color_float_array, "count")->value->ptr);
                array_reserve(mesh->colors, count / 3);
                __push_vec3_array(mesh->colors, source_color_float_array->value);
                }

                count = atoi(xml_find_attribute(polylist, "count")->value->ptr);
                array_reserve(mesh->vcount, count);
                __push_int_array(mesh->vcount, vcount->value);

                /* each triangle define by vcount has 3 vertices, each vertex has 1 position and 1 normal */
                if(!input_texcoord && !input_color) count *= 6;
                /* each triangle define by vcount has 3 vertices, each vertex has 1 position and 1 normal and 1 texcoord */
                else if(input_texcoord && !input_color) count *= 9;
                else if(!input_texcoord && input_color) count *= 9;
                else if(input_texcoord && input_color) count *= 12;
                array_reserve(mesh->p, count);
                __push_int_array(mesh->p, p->value);
        }
}

static inline struct dae_mesh *__dae_mesh_alloc()
{
        struct dae_mesh *p = smalloc(sizeof(struct dae_mesh));
        p->vertex_1 = array_alloc(sizeof(union vec3), ORDERED);
        p->vertex_2 = array_alloc(sizeof(union vec3), ORDERED);
        p->vertex_3 = array_alloc(sizeof(union vec3), ORDERED);
        p->normal_1 = array_alloc(sizeof(union vec3), ORDERED);
        p->normal_2 = array_alloc(sizeof(union vec3), ORDERED);
        p->normal_3 = array_alloc(sizeof(union vec3), ORDERED);
        return p;
}

static struct dae_mesh *__geo_mesh_to_dae_mesh(struct geo_mesh *m)
{
        struct dae_mesh *p      = __dae_mesh_alloc();
        int has_uvs             = m->uvs->len ? 1 : 0;
        int has_color           = m->colors->len ? 1: 0;
        int one_vertex_size     = 6;
        int num_attributes      = 2;
        if(has_color) {
                one_vertex_size += 4;
                num_attributes  += 1;
        }
        if(has_uvs) {
                one_vertex_size += 2;
                num_attributes  += 1;
        }

        int i, step;
        for(i = 0, step = 0; i < m->p->len; i += num_attributes) {
                /* currently I'm testing only with vertex and normal */
                int vertex_id   = array_get(m->p, int, i);
                int normal_id   = array_get(m->p, int, i + 1);

                union vec3 pos  = array_get(m->positions, union vec3, vertex_id);
                union vec3 nor  = array_get(m->normals, union vec3, normal_id);
                switch (step) {
                        case 0:
                                array_push(p->vertex_1, &pos);
                                array_push(p->normal_1, &nor);
                                break;
                        case 1:
                                array_push(p->vertex_2, &pos);
                                array_push(p->normal_2, &nor);
                                break;
                        case 2:
                                array_push(p->vertex_3, &pos);
                                array_push(p->normal_3, &nor);
                                break;
                }
                step++;
                step %= 3;
        }

        return p;
}

struct dae_mesh *dae_mesh_alloc(char *file)
{
        struct xml_element *xml         = xml_parse(file);

        struct array *geometries        = array_alloc(sizeof(struct geo_mesh *), ORDERED);
        __parse_geometry(geometries, xml);

        struct dae_mesh *p              = __geo_mesh_to_dae_mesh(array_get(geometries, struct geo_mesh *, 0));

        array_deep_free(geometries, struct geo_mesh *, __geo_mesh_free);
        xml_free(xml);

        return p;
}

void dae_mesh_free(struct dae_mesh *p)
{
        array_free(p->vertex_1);
        array_free(p->vertex_2);
        array_free(p->vertex_3);
        array_free(p->normal_1);
        array_free(p->normal_2);
        array_free(p->normal_3);
        sfree(p);
}
