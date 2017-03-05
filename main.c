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
#include <cherry/memory.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <cherry/game/game.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include <cherry/list.h>

int main(int args, char **argv)
{
        /* setup window parameters */
        int width = 480, height = 800, fullscreen = 0;
        i16 i;
        for_i(i, args - 1) {
                char* t = argv[i];
                char* v = argv[i + 1];
                if(strcmp(t, "-m") == 0) {
                        if(strcmp(v, "fullscreen") == 0) {
                                fullscreen = 1;
                        }
                } else if(strcmp(t, "-w") == 0) {
                        width = atoi(v);
                } else if(strcmp(t, "-h") == 0) {
                        height = atoi(v);
                }
        }
        video_width = width;
        video_height = height;

        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                video_width, video_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        SDL_GLContext context = SDL_GL_CreateContext(window);
        SDL_Event windowEvent;

        if(fullscreen) {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
        glViewport(0, 0, video_width, video_height);


        struct game *game = game_alloc();

        /* main loop */
        while (1) {
                if (SDL_PollEvent(&windowEvent)) {
                        if (windowEvent.type == SDL_QUIT) break;
                        if (windowEvent.type == SDL_KEYUP &&
                                windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
                        if(windowEvent.type == SDL_WINDOWEVENT) {
                                if(windowEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                                        SDL_GetWindowSize(window, &video_width, &video_height);
                                        glViewport(0, 0, video_width, video_height);
                                        game_resize(game, video_width, video_height);
                                }
                        }
                }
                game_update(game);
                if(game->can_draw) {
                        game_render(game);
                }
                SDL_GL_SwapWindow(window);
        }
        game_free(game);
        SDL_GL_DeleteContext(context);
        SDL_Quit();
        /* destroy cache and free memory pages allocated */
        cache_free();
        dim_memory();
        return 0;
}
