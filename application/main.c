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
#include <cherry/platform.h>
#include <game.h>

#if OS == WEB
        #include <GL/glew.h>
        #include <GL/glfw.h>
        #include <emscripten/emscripten.h>
        #include <emscripten/html5.h>
        #include <pthread.h>
        #include <cherry/lock.h>
#else
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_opengl.h>
        #include <SDL2/SDL_image.h>
#endif

#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include <cherry/list.h>

#if OS == WEB

struct touch_event {
        struct list_head head;
        struct event e;
};

static struct game *game = NULL;
static struct list_head touch_list;
static spin_lock lock;

static void push_mouse_event(int x, int y, int state)
{
        struct touch_event *te  = smalloc(sizeof(struct touch_event));
        te->e.type              = EVENT_MOUSE;
        te->e.mouse_x           = x;
        te->e.mouse_y           = y;
        te->e.mouse_state       = state;

        spin_lock_lock(&lock);
        list_add_tail(&te->head, &touch_list);
        spin_lock_unlock(&lock);
}

static int mouse_down(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
        push_mouse_event(e->targetX, e->targetY, MOUSE_DOWN);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int mouse_up(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
        push_mouse_event(e->targetX, e->targetY, MOUSE_UP);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int mouse_move(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
        push_mouse_event(e->targetX, e->targetY, MOUSE_MOVE);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int mouse_cancel(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
        push_mouse_event(e->targetX, e->targetY, MOUSE_CANCEL);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static void push_touch_event(int x, int y, int state)
{
        struct touch_event *te  = smalloc(sizeof(struct touch_event));
        te->e.type              = EVENT_TOUCH;
        te->e.touch_x           = x;
        te->e.touch_y           = y;
        te->e.touch_state       = state;

        spin_lock_lock(&lock);
        list_add_tail(&te->head, &touch_list);
        spin_lock_unlock(&lock);
}

static int touch_down(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
        push_touch_event(e->touches[0].targetX, e->touches[0].targetY, TOUCH_DOWN);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int touch_up(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
        push_touch_event(e->touches[0].targetX, e->touches[0].targetY, TOUCH_UP);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int touch_move(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
        push_touch_event(e->touches[0].targetX, e->touches[0].targetY, TOUCH_MOVE);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

static int touch_cancel(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
        push_touch_event(e->touches[0].targetX, e->touches[0].targetY, TOUCH_CANCEL);
        return EMSCRIPTEN_RESULT_SUCCESS;
}

int init_gl()
{
        const int width = 500, height = 750;

        if (glfwInit() != GL_TRUE) {
                printf("glfwInit() failed\n");
                return GL_FALSE;
        }
        glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
        if (glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 0, GLFW_WINDOW) != GL_TRUE) {
                printf("glfwOpenWindow() failed\n");
                return GL_FALSE;
        }

        video_width = width;
        video_height = height;
        glViewport(0, 0, video_width, video_height);

        game = game_alloc();
        INIT_LIST_HEAD(&touch_list);
        spin_lock_init(&lock, 0);

        emscripten_set_mousedown_callback("#canvas", NULL, 1, mouse_down);
        emscripten_set_mouseup_callback("#canvas", NULL, 1, mouse_up);
        emscripten_set_mousemove_callback("#canvas", NULL, 1, mouse_move);
        emscripten_set_mouseout_callback("#canvas", NULL, 1, mouse_cancel);
        emscripten_set_mouseleave_callback("#canvas", NULL, 1, mouse_cancel);

        emscripten_set_touchstart_callback("#canvas", NULL, 1, touch_down);
        emscripten_set_touchend_callback("#canvas", NULL, 1, touch_up);
        emscripten_set_touchmove_callback("#canvas", NULL, 1, touch_move);
        emscripten_set_touchcancel_callback("#canvas", NULL, 1, touch_cancel);
        return GL_TRUE;
}

void do_frame()
{
        /*
         * process mouse event
         */
        struct list_head *head = NULL;
get_touch:;
        spin_lock_lock(&lock);
        if(!list_singular(&touch_list)) {
                head = touch_list.next;
                list_del(head);
        }
        spin_lock_unlock(&lock);

        if(head) {
                struct touch_event *te = (struct touch_event *)
                        ((void *)head - offsetof(struct touch_event, head));
                game_read_event(game, &te->e);
                sfree(te);
                head = NULL;
                goto get_touch;
        }

        /*
         * update game
         */
        game_update(game);
        if(game->can_draw) {
                /*
                 * render game if needed
                 */
               game_render(game);
        }
        glfwSwapBuffers();
}

void shutdown_gl()
{
        if(game) {
                game_free(game);
                /* destroy cache and free memory pages allocated */
                cache_free();
                dim_memory();
                game = NULL;
        }
        glfwTerminate();
}

int main(int args, char **argv)
{
        if (init_gl() == GL_TRUE) {
                emscripten_set_main_loop(do_frame, 0, 1);
        }

        shutdown_gl();

        return 0;
}

#else

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
                while (SDL_PollEvent(&windowEvent)) {
                        if (windowEvent.type == SDL_QUIT) goto exit;
                        else if (windowEvent.type == SDL_KEYUP &&
                                windowEvent.key.keysym.sym == SDLK_ESCAPE) goto exit;
                        else if(windowEvent.type == SDL_WINDOWEVENT) {
                                if(windowEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                                        SDL_GetWindowSize(window, &video_width, &video_height);
                                        glViewport(0, 0, video_width, video_height);
                                        game_resize(game, video_width, video_height);
                                }
                        } else if(windowEvent.type == SDL_MOUSEBUTTONDOWN
                    		|| windowEvent.type == SDL_MOUSEBUTTONUP
                    		|| windowEvent.type == SDL_MOUSEMOTION) {
                                if(windowEvent.button.button == SDL_BUTTON_LEFT) {
                                        struct event e = {
                                                .type    = EVENT_MOUSE,
                                                .mouse_x = windowEvent.button.x,
                                                .mouse_y = windowEvent.button.y,
                                                .mouse_state = windowEvent.type == SDL_MOUSEBUTTONDOWN
                                                        ? MOUSE_DOWN
                                                        : (
                                                                windowEvent.type == SDL_MOUSEBUTTONUP
                                                                ? MOUSE_UP
                                                                : MOUSE_MOVE
                                                        )
                                        };
                                        game_read_event(game, &e);
                                }
                        }
                }
                game_update(game);
                // if(game->can_draw) {
                        game_render(game);
                // }
                SDL_GL_SwapWindow(window);
        }
exit:;
        game_free(game);
        SDL_GL_DeleteContext(context);
        SDL_Quit();
        /* destroy cache and free memory pages allocated */
        cache_free();
        dim_memory();
        return 0;
}

#endif
