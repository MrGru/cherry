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
@end

#endif
