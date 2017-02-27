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

#import "GameController.h"
#import <cherry/memory.h>
#import "GameView.h"

@interface GameController () {
    GameView *glview;
    bool updateViewport;
}

@end

@implementation GameController

- (void)viewDidLoad {
    [super viewDidLoad];

    glview = (GameView*)self.view;
    glview.delegate = self;
    glview.gameviewdelegate = self;
    glview.contentScaleFactor = 2.75f;
    updateViewport = TRUE;

    self.delegate = self; // 3
    self.preferredFramesPerSecond = 60; // 4

    video_width = [[UIScreen mainScreen] bounds].size.width;
    video_height = [[UIScreen mainScreen] bounds].size.height;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if(_game) {
        game_render(_game);
    }
}

- (void)glkViewControllerUpdate:(GLKViewController *)controller
{
    if(updateViewport) {
        glViewport(0, 0, video_width, video_height);
    }
    if(!_game) {
        _game = game_alloc();
    } else {
        game_update(_game);
    }
    if(updateViewport) {
        game_resize(_game, video_width, video_height);
    }
    updateViewport = FALSE;
}

-(void)reshape:(GameView *)view
{
    video_width = view.bounds.size.width;
    video_height = view.bounds.size.height;
    updateViewport = TRUE;
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)destroyGame
{
    if(_game) {
        game_free(_game);
        _game = NULL;

        cache_free();
        dim_memory();
    }
}

@end
