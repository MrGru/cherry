//
//  ViewController.m
//  game
//
//  Created by Manh Tran on 12/26/16.
//  Copyright © 2016 Manh Tran. All rights reserved.
//

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
