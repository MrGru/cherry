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
#include <cherry/graphic/image.h>

#if IMAGE_TYPE == IMAGE_SDL2

#include <cherry/memory.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/*
 * allocate new image object from file path
 */
struct image *image_alloc(char *file)
{
        /* try create surface from file */
        SDL_Surface *tempimage = IMG_Load(file);
        if( ! tempimage) return 0;

        /* fix byte order */
        SDL_Surface *tempsurface;
        Uint32 rmask, gmask, bmask, amask;

        /* fix ios color */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif
        tempsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, tempimage->w, tempimage->h, 32, rmask, gmask, bmask, amask);
        SDL_BlitSurface(tempimage, NULL, tempsurface, NULL);
        SDL_FreeSurface(tempimage);

        tempimage = tempsurface;

        struct image *p = smalloc(sizeof(struct image));
        p->ptr = tempimage;

        if(tempimage->format->BytesPerPixel == 3)
        {
                p->type = GL_RGB;
        }
        else
        {
                p->type = GL_RGBA;
        }

        return p;
}

/*
 * deallocate image object
 */
void image_free(struct image *p)
{
        if(p->ptr) {
                SDL_FreeSurface(p->ptr);
        }
        sfree(p);
}

/*
 * get image width
 */
u16 image_width(struct image *p)
{
        return ((SDL_Surface *)p->ptr)->w;
}

/*
 * get image height
 */
u16 image_height(struct image *p)
{
        return ((SDL_Surface *)p->ptr)->h;
}

/*
 * get image pixels
 */
void *image_pixels(struct image *p)
{
        return ((SDL_Surface *)p->ptr)->pixels;
}

/*
 * get image type
 */
i32 image_type(struct image *p)
{
        return p->type;
}

#endif
