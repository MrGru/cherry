//
//  AppDelegate.h
//  ios-dynamic-loading-framework
//
//  Created by Patrik Nyblad on 18/05/16.
//  Copyright © 2016 CarmineStudios. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GameControllerProtocol.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) UIViewController *gamecontroller;
@property id <GameControllerDelegate> gamedelegate;
@property void *gamecode;

@end

