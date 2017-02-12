#import "GameView.h"

@implementation GameView

- (void)initCommon
{
    EAGLContext *glctx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    self.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    self.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
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
