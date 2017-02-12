#import <GLKit/GLKit.h>

@protocol GameViewDelegate;

@interface GameView : GLKView

@property (nonatomic, weak) id <GameViewDelegate> gameviewdelegate;

@end

@protocol GameViewDelegate <NSObject>

@required
// called if the view changes orientation or size, renderer can precompute its view and projection matricies here for example
- (void)reshape:(GameView *)view;


@end
