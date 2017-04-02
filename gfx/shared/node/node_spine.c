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
#include <cherry/graphic/node.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/bytes.h>
#include <cherry/math/math.h>
#include <cherry/graphic/shader.h>
#include <cherry/graphic/uniform.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/device_buffer.h>
#include <cherry/graphic/update_task.h>

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>

#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1800
#endif

static int              buffer_index = 0;
static union vec2       buffer_position[SPINE_MESH_VERTEX_COUNT_MAX/2];
static union vec2       buffer_texcoord[SPINE_MESH_VERTEX_COUNT_MAX/2];
static union vec4       buffer_color[SPINE_MESH_VERTEX_COUNT_MAX/2];

static float worldVertices[SPINE_MESH_VERTEX_COUNT_MAX];

struct spine_data {
        struct update_task      *task;
        AnimationState          *state;
        Skeleton                *skeleton;
        float                   timeScale;
        struct string           *spine_file;
        struct string           *atlas_file;
};

static void __spin_data_free(struct spine_data *p)
{
        if(p->task) {
                update_task_free(p->task);
        }
        AnimationState_dispose(p->state);
        Skeleton_dispose(p->skeleton);
        string_free(p->spine_file);
        string_free(p->atlas_file);
        sfree(p);
}

static inline void __node_update_spine_data(struct node *p, float time)
{
        struct spine_data *data = p->data;
        if(data) {
                Skeleton_update(data->skeleton, time);
                AnimationState_update(data->state, time * data->timeScale);
                AnimationState_apply(data->state, data->skeleton);
                Skeleton_updateWorldTransform(data->skeleton);
        }
}

static inline void __setup_render_buffer(struct node *p, struct device_buffer_group *group, u16 item_size, u8 location, u32 key)
{
        struct device_buffer *db = device_buffer_alloc(BUFFER_VERTICE, item_size, location);
        device_buffer_group_add(group, db);
#if GFX == OGL
        node_setup_buffer(db, p->current_shader, key);
#endif
}

static void __setup_render_group(struct node *p, struct node_render_buffer_group *group)
{
        device_buffer_group_bind_construct(group->group);
        __setup_render_buffer(p, group->group, sizeof(union vec2), BUFFER_DEVICE, INPUT_POSITION_2D);
        __setup_render_buffer(p, group->group, sizeof(union vec2), BUFFER_DEVICE, INPUT_TEXCOORD);
        __setup_render_buffer(p, group->group, sizeof(union vec4), BUFFER_DEVICE, INPUT_COLOR);
        device_buffer_group_bind_construct(NULL);
}

static int __build_current_spin_buffer(struct node *p, int *id_buffer, u32 blend, struct texture *tex, u8 frame)
{
        if(!buffer_index) return 0;

        int id = *id_buffer;

        struct node_render_content *content = p->current_render_content[frame];
        node_render_content_reserve(content, id + 1);

        struct node_render_buffer_group *group  = array_get(content->buffer_groups,
                struct node_render_buffer_group *, id);
        group->vertice_count                    = buffer_index;
        group->blend_mode                       = blend;
        node_render_buffer_group_add_texture(group, tex);

        if(group->group->buffers->len == 0) __setup_render_group(p, group);

        struct device_buffer *position_buffer   = array_get(group->group->buffers, struct device_buffer *, 0);
        struct device_buffer *texcoord_buffer   = array_get(group->group->buffers, struct device_buffer *, 1);
        struct device_buffer *color_buffer      = array_get(group->group->buffers, struct device_buffer *, 2);

        u64 position_buffer_size                = buffer_index * sizeof(union vec2);
        if(position_buffer_size <= position_buffer->size) {
                device_buffer_sub(position_buffer, 0, buffer_position, buffer_index * sizeof(union vec2));
                device_buffer_sub(texcoord_buffer, 0, buffer_texcoord, buffer_index * sizeof(union vec2));
                device_buffer_sub(color_buffer, 0, buffer_color, buffer_index * sizeof(union vec4));
        } else {
                device_buffer_fill(position_buffer, buffer_position, buffer_index * sizeof(union vec2));
                device_buffer_fill(texcoord_buffer, buffer_texcoord, buffer_index * sizeof(union vec2));
                device_buffer_fill(color_buffer, buffer_color, buffer_index * sizeof(union vec4));
        }

        buffer_index = 0;
        id++;
        *id_buffer = id;

        return 1;
}

static inline void __append_buffer(struct node *p, int *id_buffer, u32 blend, struct texture *tex, u8 frame,
        float r, float g, float b, float a, float x, float y, float t, float s)
{
        buffer_position[buffer_index].x = x;
        buffer_position[buffer_index].y = y;
        buffer_texcoord[buffer_index].t = t;
        buffer_texcoord[buffer_index].s = s;
        buffer_color[buffer_index].r    = r;
        buffer_color[buffer_index].g    = g;
        buffer_color[buffer_index].b    = b;
        buffer_color[buffer_index].a    = a;
        buffer_index++;
        if(buffer_index == SPINE_MESH_VERTEX_COUNT_MAX/2) {
                __build_current_spin_buffer(p, id_buffer, blend, tex, frame);
        }
}

static void __node_setup_spine_buffer(struct node *p, u8 frame)
{
        int i, j;
        int id_buffer = 0;
        struct texture *tex = NULL;
        struct node_render_content *content = p->current_render_content[frame];
        node_render_content_clear_keep(content);

        buffer_index = 0;
        struct spine_data *data = p->data;
        if(!data) goto end;

setup:;
        u32 current_blend = BLEND_NONE;
        for_i(i, data->skeleton->slotsCount) {
                Slot *slot              = data->skeleton->drawOrder[i];
                Attachment *attachment  = slot->attachment;
                if(!attachment) continue;

                u32 blend;
                switch (slot->data->blendMode) {
                        case BLEND_MODE_ADDITIVE:
                                blend = BLEND_ADDITIVE;
                                break;
                        case BLEND_MODE_MULTIPLY:
                                blend = BLEND_MULTIPLY;
                                break;
                        case BLEND_MODE_SCREEN:
                        default:
                                blend = BLEND_ALPHA;
                }
                if(current_blend != blend) {
                        __build_current_spin_buffer(p, &id_buffer, blend, tex, frame);
                        current_blend = blend;
                }
                if(attachment->type == ATTACHMENT_REGION) {
                        RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
                        tex = (struct texture *)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
                        RegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);
                        float r = data->skeleton->r * slot->r;
                        float g = data->skeleton->g * slot->g;
                        float b = data->skeleton->b * slot->b;
                        float a = data->skeleton->a * slot->a;
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X1], worldVertices[VERTEX_Y1],
                                regionAttachment->uvs[VERTEX_X1], regionAttachment->uvs[VERTEX_Y1]);
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X2], worldVertices[VERTEX_Y2],
                                regionAttachment->uvs[VERTEX_X2], regionAttachment->uvs[VERTEX_Y2]);
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X3], worldVertices[VERTEX_Y3],
                                regionAttachment->uvs[VERTEX_X3], regionAttachment->uvs[VERTEX_Y3]);
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X1], worldVertices[VERTEX_Y1],
                                regionAttachment->uvs[VERTEX_X1], regionAttachment->uvs[VERTEX_Y1]);
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X3], worldVertices[VERTEX_Y3],
                                regionAttachment->uvs[VERTEX_X3], regionAttachment->uvs[VERTEX_Y3]);
                        __append_buffer(p, &id_buffer, blend, tex, frame,
                                r, g, b, a, worldVertices[VERTEX_X4], worldVertices[VERTEX_Y4],
                                regionAttachment->uvs[VERTEX_X4], regionAttachment->uvs[VERTEX_Y4]);
                } else if(attachment->type == ATTACHMENT_MESH) {
                        MeshAttachment* mesh = (MeshAttachment*)attachment;
                        if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) {
                                continue;
                        }

                        tex = (struct texture *)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;
                        MeshAttachment_computeWorldVertices(mesh, slot, worldVertices);
                        float r = data->skeleton->r * slot->r;
                        float g = data->skeleton->g * slot->g;
                        float b = data->skeleton->b * slot->b;
                        float a = data->skeleton->a * slot->a;
                        for_i(j, mesh->trianglesCount) {
                                int index = mesh->triangles[j] << 1;
                                __append_buffer(p, &id_buffer, blend, tex, frame,
                                        r, g, b, a, worldVertices[index], worldVertices[index + 1],
                                        mesh->uvs[index], mesh->uvs[index + 1]);
                        }
                }
        }
        __build_current_spin_buffer(p, &id_buffer, current_blend, tex, frame);
        content->actives = id_buffer;
end:;
}

void node_update_spine(struct node *p, float time, u8 frame)
{
        if(p->type == NODE_SPINE) {
                __node_update_spine_data(p, time);
                __node_setup_spine_buffer(p, frame);
        }
}

void node_spine_set_animation(struct node * p, int trackindex, char *name, int loop)
{
        if(p->type == NODE_SPINE) {
                struct spine_data *data = p->data;
                AnimationState_setAnimationByName(data->state, trackindex, name, loop);
        }
}

void node_spine_add_animation(struct node * p, int trackindex, char *name, int loop, float delay)
{
        if(p->type == NODE_SPINE) {
                struct spine_data *data = p->data;
                AnimationState_addAnimationByName(data->state, trackindex, name, loop, delay);
        }
}

void node_spine_run_animation(struct node *p)
{
        if(p->type == NODE_SPINE) {
                struct spine_data *data = p->data;
                if( ! data->task) {
                        data->task = update_task_request(p->manager, p, (update_task_callback) node_update_spine, -1);
                }
        }
}

void node_spine_stop_animation(struct node *p)
{
        if(p->type == NODE_SPINE) {
                struct spine_data *data = p->data;
                if(data->task) {
                        update_task_free(data->task);
                        data->task = NULL;
                }
        }
}

/*
 * show spine
 */

struct spine_cache {
        SkeletonData            *skeletonData;
        AnimationStateData      *stateData;
};

static void __spine_cache_free(struct spine_cache *p)
{
        SkeletonData_dispose(p->skeletonData);
        AnimationStateData_dispose(p->stateData);
        sfree(p);
}

static struct map *cache = NULL;
static struct map *atlas_cache = NULL;

static void __free_spine_cache()
{
        map_deep_free(cache, struct spine_cache *, __spine_cache_free);
        cache = NULL;

        map_deep_free(atlas_cache, Atlas *, Atlas_dispose);
        atlas_cache = NULL;
}

static SkeletonData* readSkeletonJsonData (const char* filename, Atlas* atlas, float scale) {
	SkeletonJson* json = SkeletonJson_create(atlas);
	json->scale = scale;
	SkeletonData* skeletonData = SkeletonJson_readSkeletonDataFile(json, filename);
	if (!skeletonData) {
		debug("%s\n", json->error);
		exit(0);
	}
	SkeletonJson_dispose(json);
	return skeletonData;
}

static SkeletonData* readSkeletonBinaryData (const char* filename, Atlas* atlas, float scale) {
	SkeletonBinary* binary = SkeletonBinary_create(atlas);
	binary->scale = scale;
	SkeletonData *skeletonData = SkeletonBinary_readSkeletonDataFile(binary, filename);
	if (!skeletonData) {
		debug("%s\n", binary->error);
		exit(0);
	}
	SkeletonBinary_dispose(binary);
	return skeletonData;
}

void node_show_spine(struct node *p, u32 shader_type, char* spine_file, char *atlas_file, float scale)
{
        if(!cache) {
                cache = map_alloc(sizeof(struct spine_cache *));
                cache_add(__free_spine_cache);

                atlas_cache = map_alloc(sizeof(Atlas *));
        }

        struct spine_data *data = p->data;
        int i;
        switch (p->type) {
                case NODE_SPINE:
                        goto check_file;
                default:
                        goto setup_input;
        }
fail:;
        p->type                 = -1;
        p->shader_type          = -1;
        p->current_shader       = NULL;
        for_i(i, BUFFERS) {
                if(p->current_render_content[i]) {
                        node_render_content_clear(p->current_render_content[i]);
                }
        }
        array_force_len(p->current_common_uniform_buffers, 0);
        array_force_len(p->current_uniform_buffers, 0);
        if(data) {
                p->data_free(p->data);
                p->data = NULL;
        }
        return;

check_file:;
        if(data && strcmp(data->spine_file->ptr, spine_file) == 0
                && strcmp(data->atlas_file->ptr, atlas_file) == 0) goto end;

setup_input:;
        p->type                 = NODE_SPINE;
        p->shader_type          = shader_type;
        p->current_shader       = map_get(p->manager->shaders, struct shader *, &shader_type, sizeof(shader_type));
        if(!p->current_shader) goto fail;

        for_i(i, BUFFERS) {
                if(p->current_render_content[i]) {
                        node_render_content_clear(p->current_render_content[i]);
                } else {
                        p->current_render_content[i] = node_render_content_alloc();
                }
        }
        switch (shader_type) {
                case SHADER_2D_TEXTURE_COLOR:
                        break;
                default:
                        goto fail;
        }


        if(data) __spin_data_free(data);
        data                    = smalloc(sizeof(struct spine_data));
        p->data                 = data;
        p->data_free            = (void(*)(void*))__spin_data_free;

        data->task       = NULL;
        data->timeScale         = 1;
        data->spine_file        = string_alloc_chars(spine_file, strlen(spine_file));
        data->atlas_file        = string_alloc_chars(atlas_file, strlen(atlas_file));

        struct bytes *key      = bytes_alloc();
        bytes_cat(key, data->spine_file->ptr, data->spine_file->len);
        bytes_cat(key, data->atlas_file->ptr, data->atlas_file->len);
        bytes_cat(key, &scale, sizeof(scale));

        if(!map_has_key(atlas_cache, data->atlas_file->ptr, data->atlas_file->len)) {
                Atlas* atlas = Atlas_createFromFile(atlas_file, 0);
                map_set(atlas_cache, data->atlas_file->ptr, data->atlas_file->len, &atlas);
        }

        if(!map_has_key(cache, key->ptr, key->len)) {
                Atlas *atlas            = map_get(atlas_cache, Atlas *, data->atlas_file->ptr, data->atlas_file->len);
                struct spine_cache *sc  = smalloc(sizeof(struct spine_cache));
                sc->skeletonData        = readSkeletonBinaryData(data->spine_file->ptr, atlas, scale);
                sc->stateData           = AnimationStateData_create(sc->skeletonData);
                map_set(cache, key->ptr, key->len, &sc);
        }
        struct spine_cache *sc  = map_get(cache, struct spine_cache *, key->ptr, key->len);
        bytes_free(key);

        data->skeleton          = Skeleton_create(sc->skeletonData);
        data->state             = AnimationState_create(sc->stateData);
        Skeleton_updateWorldTransform(data->skeleton);

setup_uniform:;
        if(!node_setup_common_uniform(p)) goto fail;
        if(!node_setup_uniform(p)) goto fail;
        return;
end:;
}
