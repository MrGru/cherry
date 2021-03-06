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
#ifndef __CHERRY_APP_RENDERER__
#define __CHERRY_APP_RENDERER__
#import <Foundation/Foundation.h>
#import "AppView.h"
#import "AppViewController.h"

@interface AppRenderer : NSObject<AppViewDelegate, AppViewControllerDelegate>

- (void)configure:(AppView *)view;

@end
#endif
