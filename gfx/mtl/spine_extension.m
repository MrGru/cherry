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
#import <cherry/graphic/texture.h>

#if GFX == MTL

#define SPINE_SHORT_NAMES
#import <spine/spine.h>
#import <spine/extension.h>
#import <cherry/stdio.h>
#import <cherry/string.h>

void _AtlasPage_createTexture (AtlasPage* self, const char* path){
        struct texture *texture = texture_alloc_file((char*)path);
        texture->ref++;
	// if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) texture->setRepeated(true);

	self->rendererObject = texture;
	self->width = texture->width;
	self->height = texture->height;
}

void _AtlasPage_disposeTexture (AtlasPage* self){
        texture_free((struct texture *)self->rendererObject);
}

char* _Util_readFile (const char* path, int* length){
        struct string *content = file_full_path((char*)path);
	char* data = _readFile(content->ptr, length);
        string_free(content);
        return data;
}

#endif
