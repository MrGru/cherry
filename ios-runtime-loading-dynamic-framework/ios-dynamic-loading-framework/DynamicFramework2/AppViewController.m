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
#import "AppViewController.h"
#import "AppView.h"
#import "AppRenderer.h"
#import <QuartzCore/CAMetalLayer.h>

@implementation AppViewController
{
@private
    // app control

    CADisplayLink *_displayLink;

    // boolean to determine if the first draw has occured
    BOOL _firstDrawOccurred;

    CFTimeInterval _timeSinceLastDrawPreviousTime;

    // pause/resume
    BOOL _gameLoopPaused;

    // our renderer instance
    AppRenderer *_renderer;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver: self
                                                    name: UIApplicationDidEnterBackgroundNotification
                                                  object: nil];

    [[NSNotificationCenter defaultCenter] removeObserver: self
                                                    name: UIApplicationWillEnterForegroundNotification
                                                  object: nil];
}

- (void)dispatchGameLoop
{
    // create a game loop timer using a display link
    _displayLink = [[UIScreen mainScreen] displayLinkWithTarget:self
                                                       selector:@selector(gameloop)];
//    _displayLink.frameInterval = _interval;
    _displayLink.preferredFramesPerSecond = 60;
    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop]
                       forMode:NSDefaultRunLoopMode];
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [_delegate touchesBegan:touches withEvent:event andView:self.view];
}

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [_delegate touchesMoved:touches withEvent:event andView:self.view];
}

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [_delegate touchesEnded:touches withEvent:event andView:self.view];
}

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [_delegate touchesCancelled:touches withEvent:event andView:self.view];
}

- (void)initCommon
{
    _renderer = [AppRenderer new];
    self.delegate = _renderer;

    NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];
    //  Register notifications to start/stop drawing as this app moves into the background
    [notificationCenter addObserver: self
                           selector: @selector(didEnterBackground:)
                               name: UIApplicationDidEnterBackgroundNotification
                             object: nil];

    [notificationCenter addObserver: self
                           selector: @selector(willEnterForeground:)
                               name: UIApplicationWillEnterForegroundNotification
                             object: nil];

    _interval = 1;
}

- (id)init
{
    self = [super init];

    if(self)
    {
        [self initCommon];
    }
    return self;
}

// Called when loaded from nib
- (id)initWithNibName:(NSString *)nibNameOrNil
               bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil
                           bundle:nibBundleOrNil];

    if(self)
    {
        [self initCommon];
    }

    return self;
}

// called when loaded from storyboard
- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];

    if(self)
    {
        [self initCommon];
    }

    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    AppView *renderView = (AppView *)self.view;
//    renderView.contentScaleFactor = 2.75;
    renderView.delegate = _renderer;

    // load all renderer assets before starting game loop
    [_renderer configure:renderView];
}


// The main game loop called by the timer above
- (void)gameloop
{

    // tell our delegate to update itself here.
    [_delegate update:self];

    if(!_firstDrawOccurred)
    {
        // set up timing data for display since this is the first time through this loop
        _timeSinceLastDraw             = 0.0;
        _timeSinceLastDrawPreviousTime = CACurrentMediaTime();
        _firstDrawOccurred              = YES;
    }
    else
    {
        // figure out the time since we last we drew
        CFTimeInterval currentTime = CACurrentMediaTime();

        _timeSinceLastDraw = currentTime - _timeSinceLastDrawPreviousTime;

        // keep track of the time interval between draws
        _timeSinceLastDrawPreviousTime = currentTime;
    }

    // display (render)

    assert([self.view isKindOfClass:[AppView class]]);

    // call the display method directly on the render view (setNeedsDisplay: has been disabled in the renderview by default)
    [(AppView *)self.view display];
}

- (void)stopGameLoop
{
    if(_displayLink)
    {
        [_displayLink invalidate];
    }
}

- (void)setPaused:(BOOL)pause
{
    if(_gameLoopPaused == pause)
    {
        return;
    }

    if(_displayLink)
    {
        // inform the delegate we are about to pause
        [_delegate viewController:self
                        willPause:pause];

        if(pause == YES)
        {
            _gameLoopPaused = pause;
            _displayLink.paused   = YES;

            // ask the view to release textures until its resumed
            [(AppView *)self.view releaseTextures];
        }
        else
        {
            _gameLoopPaused = pause;
            _displayLink.paused   = NO;
        }

    }
}

- (BOOL)isPaused
{
    return _gameLoopPaused;
}

- (void)didEnterBackground:(NSNotification*)notification
{
    [self setPaused:YES];
}

- (void)willEnterForeground:(NSNotification*)notification
{
    [self setPaused:NO];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    // run the game loop
    [self dispatchGameLoop];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];

    // end the gameloop
    [self stopGameLoop];
}

-(void)destroyGame
{
    [_delegate destroyGame];
}

@end
