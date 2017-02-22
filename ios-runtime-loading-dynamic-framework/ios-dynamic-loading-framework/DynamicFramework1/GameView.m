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
#import "GameView.h"

@implementation GameView

- (void)initCommon
{
    EAGLContext *glctx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    self.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    self.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    self.drawableMultisample = 4;
    self.context = glctx;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if(self)
    {
        [self initCommon];
    }

    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];

    if(self)
    {
        [self initCommon];
    }
    return self;
}

- (void)setContentScaleFactor:(CGFloat)contentScaleFactor
{
    [super setContentScaleFactor:contentScaleFactor];
    [_gameviewdelegate reshape:self];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    [_gameviewdelegate reshape:self];
}

@end
