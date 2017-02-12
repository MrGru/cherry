#import <UIKit/UIKit.h>
#import "GameControllerProtocol.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) UIViewController *gamecontroller;
@property id <GameControllerDelegate> gamedelegate;
@property void *gamecode;

@end
