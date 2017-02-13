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
#import "AppDelegate.h"
#import "dlfcn.h"
#import "GameControllerProtocol.h"

#import <sys/utsname.h>

static NSString* deviceName()
{
    struct utsname systemInfo;
    uname(&systemInfo);

    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

static int getDeviceModelNumber(NSString *n)
{
    int i, j;
    char m[10];
    const char *s = [n UTF8String];
    for(i = 0, j = 0; i < [n length]; ++i) {
        char c = s[i];
        if(isnumber(c)) {
            m[j++] = c;
        } else if(c == ',') {
            m[j] = '\0';
            break;
        }
        if(i == [n length] - 1) m[j] = '\0';
    }
    return atoi(m);
}

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    bool metal_available = FALSE;
    NSString *framework;
    NSString *storyboardbundle;
    NSString *storyboardname;
    NSString *gamecontrolleridentifier;

    NSString *n = deviceName();
    int ni = getDeviceModelNumber(n);
    if([n containsString:@"iPhone"]) {
        metal_available = ni >= 6;
    } else if([n containsString:@"iPad"]) {
        metal_available = ni >= 4;
    } else if([n containsString:@"iPod"]) {
        metal_available = ni >= 7;
    }

    metal_available = FALSE;

    if(metal_available) {
        framework = @"DynamicFramework2.framework/DynamicFramework2";
        storyboardbundle = @"com.manh.DynamicFramework2";
        storyboardname = @"GameStoryBoard";
        gamecontrolleridentifier = @"gamecontroller";
    } else {

        framework = @"DynamicFramework1.framework/DynamicFramework1";
        storyboardbundle = @"com.manh.DynamicFramework1";
        storyboardname = @"GameStoryBoard";
        gamecontrolleridentifier = @"gamecontroller";
    }

    _gamecode = dlopen([framework UTF8String], RTLD_NOW);

    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:storyboardname bundle:[NSBundle bundleWithIdentifier:storyboardbundle]];
    UIViewController *object = [storyboard instantiateViewControllerWithIdentifier:gamecontrolleridentifier];

    _window.rootViewController = object;
    [_window makeKeyAndVisible];

    _gamecontroller = object;
    _gamedelegate = (id<GameControllerDelegate>)object;

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    [_gamedelegate destroyGame];
    dlclose(_gamecode);
}


@end
