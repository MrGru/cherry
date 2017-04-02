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
#import "AppRenderer.h"
#import "AppViewController.h"
#import "AppView.h"
#import <cherry/graphic/graphic.h>
#import <cherry/graphic/metal.h>
#import <game.h>
#import <cherry/memory.h>
#import <pthread.h>
#import <cherry/lock.h>
#import <cherry/list.h>

struct touch_event {
    struct list_head head;
    struct event e;
};

static const long kInFlightCommandBuffers = BUFFERS;

@implementation AppRenderer
{
    dispatch_semaphore_t _inflight_semaphore;
    id <MTLDevice> _device;
    id <MTLCommandQueue> _commandQueue;
    struct game *game;
    spin_lock touch_lock;
    struct list_head touch_list;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _inflight_semaphore = dispatch_semaphore_create(kInFlightCommandBuffers);
    }
    return self;
}

#pragma mark Configure

- (void)configure:(AppView *)view
{
    _device = view.device;

    shared_mtl_device = view.device;

    // setup view with drawable formats
    view.depthPixelFormat   = MTLPixelFormatDepth32Float;
    view.stencilPixelFormat = MTLPixelFormatInvalid;
    view.sampleCount        = 1;
    
    // create a new command queue
    _commandQueue = [_device newCommandQueue];

    shared_mtl_library = [_device newDefaultLibrary];

    shared_mtl_main_pass = view.renderPassDescriptor;
    
    video_width = view.bounds.size.width;
    video_height = view.bounds.size.height;
    
    game = game_alloc();
    
    INIT_LIST_HEAD(&touch_list);
    spin_lock_init(&touch_lock, 0);
}

#pragma mark Render

- (void)render:(AppView *)view
{
    if(!game) return;
    
    //if(game->can_draw) {
        dispatch_semaphore_wait(_inflight_semaphore, DISPATCH_TIME_FOREVER);
        
        id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
        shared_mtl_command_buffer = commandBuffer;
        shared_mtl_main_pass = view.renderPassDescriptor;
        
        game_render(game);
        [commandBuffer presentDrawable:view.currentDrawable];
        
        
        __block dispatch_semaphore_t block_sema = _inflight_semaphore;
        [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
            dispatch_semaphore_signal(block_sema);
        }];
        
        [commandBuffer commit];
   // }
}

- (void)reshape:(AppView *)view
{
    if(game) {
        game_resize(game, (int)view.bounds.size.width, (int)view.bounds.size.height);
    }
}

#pragma mark Update

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView *)view
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:view];
    
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

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView *)view
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:view];
    
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

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView *)view
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:view];
    
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

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView *)view
{
    UITouch *touch          = [touches anyObject];
    CGPoint point           = [touch locationInView:view];
    
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

- (void)update:(AppViewController *)controller
{
    if(!game) return;
    
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
        game_read_event(game, &te->e);
        sfree(te);
        head = NULL;
        goto get_touch;
    }

    game_update(game);
}

- (void)viewController:(AppViewController *)controller willPause:(BOOL)pause
{

}

-(void)destroyGame
{
    if(game) {
        game_free(game);
        game = NULL;

        cache_free();
        dim_memory();
    }
}


@end
