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
#import <pthread.h>
#import <cherry/lock.h>
#import <cherry/list.h>

struct touch_event {
    struct list_head head;
    struct event e;
};


@interface GameController () {
    GameView *glview;
    bool updateViewport;
    CADisplayLink *link;
    spin_lock touch_lock;
    struct list_head touch_list;
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
    
    link = [CADisplayLink displayLinkWithTarget:self selector:@selector(handleDisplayLink:)];
    link.preferredFramesPerSecond = 60;
    [link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    
    INIT_LIST_HEAD(&touch_list);
    spin_lock_init(&touch_lock, 0);
    
    _game = game_alloc();
}

- (void)handleDisplayLink:(CADisplayLink *)displayLink
{
    [self glkViewControllerUpdate:self];
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:self.view];
    
    struct touch_event *te  = smalloc(sizeof(struct touch_event));
    te->e.type              = EVENT_TOUCH;
    te->e.touch_x           = point.x;
    te->e.touch_y           = point.y;
    te->e.touch_state       = TOUCH_DOWN;
    INIT_LIST_HEAD(&te->head);
    
    spin_lock_lock(&touch_lock);
    list_add_tail(&te->head, &touch_list);
    spin_lock_unlock(&touch_lock);
}

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:self.view];
    
    struct touch_event *te  = smalloc(sizeof(struct touch_event));
    te->e.type              = EVENT_TOUCH;
    te->e.touch_x           = point.x;
    te->e.touch_y           = point.y;
    te->e.touch_state       = TOUCH_MOVE;
    INIT_LIST_HEAD(&te->head);
    
    spin_lock_lock(&touch_lock);
    list_add_tail(&te->head, &touch_list);
    spin_lock_unlock(&touch_lock);
}

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:self.view];
    
    struct touch_event *te  = smalloc(sizeof(struct touch_event));
    te->e.type              = EVENT_TOUCH;
    te->e.touch_x           = point.x;
    te->e.touch_y           = point.y;
    te->e.touch_state       = TOUCH_UP;
    INIT_LIST_HEAD(&te->head);
    
    spin_lock_lock(&touch_lock);
    list_add_tail(&te->head, &touch_list);
    spin_lock_unlock(&touch_lock);
}

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:self.view];
    
    struct touch_event *te  = smalloc(sizeof(struct touch_event));
    te->e.type              = EVENT_TOUCH;
    te->e.touch_x           = point.x;
    te->e.touch_y           = point.y;
    te->e.touch_state       = TOUCH_CANCEL;
    INIT_LIST_HEAD(&te->head);
    
    spin_lock_lock(&touch_lock);
    list_add_tail(&te->head, &touch_list);
    spin_lock_unlock(&touch_lock);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if(_game) {
        game_render(_game);
    }
}


- (void)glkViewControllerUpdate:(GLKViewController *)controller
{
    if(!_game) return;
    
    if(updateViewport) {
        glViewport(0, 0, video_width, video_height);
    }

    struct list_head *head = NULL;
get_touch:;
    spin_lock_lock(&touch_lock);
    if(!list_singular(&touch_list)) {
        head = touch_list.next;
        list_del(head);
    }
    spin_lock_unlock(&touch_lock);
    
    if(head) {
        struct touch_event *te = (struct touch_event *)
            ((void *)head - offsetof(struct touch_event, head));
        game_read_event(_game, &te->e);
        sfree(te);
        head = NULL;
        goto get_touch;
    }
    game_update(_game);
    
    if(updateViewport) {
        game_resize(_game, video_width, video_height);
    }
    updateViewport = FALSE;
    if(_game->can_draw) {
        [self.view setNeedsDisplay];
    }
}

-(void)reshape:(GameView *)view
{
    video_width = view.bounds.size.width;
    video_height = view.bounds.size.height;
    updateViewport = TRUE;
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    // Pause the rendering loop to draw on demand.
    self.paused = YES;
    self.resumeOnDidBecomeActive = NO;
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
