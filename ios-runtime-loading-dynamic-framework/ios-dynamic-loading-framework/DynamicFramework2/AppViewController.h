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
#ifndef __M_APP_VIEW_CONTROLLER_H__
#define __M_APP_VIEW_CONTROLLER_H__

#import <UIKit/UIKit.h>
#import "GameControllerProtocol.h"

@protocol AppViewControllerDelegate;

@interface AppViewController : UIViewController<GameControllerDelegate>

@property (nonatomic, weak) id <AppViewControllerDelegate> delegate;

@property (nonatomic, readonly) NSTimeInterval timeSinceLastDraw;

@property (nonatomic) NSUInteger interval;

@property (nonatomic, getter=isPaused) BOOL paused;

- (void)stopGameLoop;

@end

@protocol AppViewControllerDelegate <NSObject>
@required

// Note this method is called from the thread the main game loop is run
- (void)update:(AppViewController *)controller;

// called whenever the main game loop is paused, such as when the app is backgrounded
- (void)viewController:(AppViewController *)controller willPause:(BOOL)pause;

- (void)destroyGame;

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView*)view;

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView*)view;

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView*)view;

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event andView:(UIView*)view;

@end

#endif
