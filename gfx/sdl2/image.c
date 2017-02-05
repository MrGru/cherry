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

        struct image *p = smalloc(sizeof(struct image));
        p->ptr = tempsurface;

        /* Extracting color components from a 32-bit color value */
        SDL_PixelFormat *fmt;
        SDL_Surface *surface = tempsurface;
        Uint32 temp, pixel;
        Uint8 red, green, blue, alpha;

        fmt = surface->format;
        SDL_LockSurface(surface);
        pixel = *((Uint32*)surface->pixels);
        SDL_UnlockSurface(surface);

        /* Get Red component */
        temp = pixel & fmt->Rmask;  /* Isolate red component */
        temp = temp >> fmt->Rshift; /* Shift it down to 8-bit */
        temp = temp << fmt->Rloss;  /* Expand to a full 8-bit number */
        red = (Uint8)temp;

        /* Get Green component */
        temp = pixel & fmt->Gmask;  /* Isolate green component */
        temp = temp >> fmt->Gshift; /* Shift it down to 8-bit */
        temp = temp << fmt->Gloss;  /* Expand to a full 8-bit number */
        green = (Uint8)temp;

        /* Get Blue component */
        temp = pixel & fmt->Bmask;  /* Isolate blue component */
        temp = temp >> fmt->Bshift; /* Shift it down to 8-bit */
        temp = temp << fmt->Bloss;  /* Expand to a full 8-bit number */
        blue = (Uint8)temp;

        /* Get Alpha component */
        temp = pixel & fmt->Amask;  /* Isolate alpha component */
        temp = temp >> fmt->Ashift; /* Shift it down to 8-bit */
        temp = temp << fmt->Aloss;  /* Expand to a full 8-bit number */
        alpha = (Uint8)temp;
        if(alpha == 0)
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
