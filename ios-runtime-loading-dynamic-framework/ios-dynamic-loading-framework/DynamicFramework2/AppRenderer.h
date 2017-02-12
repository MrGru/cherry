#import <Foundation/Foundation.h>
#import "AppView.h"
#import "AppViewController.h"

@interface AppRenderer : NSObject<AppViewDelegate, AppViewControllerDelegate>

- (void)configure:(AppView *)view;

@end
