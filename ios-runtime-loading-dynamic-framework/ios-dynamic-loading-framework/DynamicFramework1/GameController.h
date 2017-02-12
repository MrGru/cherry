#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <cherry/game/game.h>
#import "GameControllerProtocol.h"
#import "GameView.h"

@interface GameController : GLKViewController<GLKViewDelegate, GLKViewControllerDelegate, GameControllerDelegate, GameViewDelegate>

@property struct game *game;


@end

