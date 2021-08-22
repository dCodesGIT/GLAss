//
//  AppDelegate.m
//  Event handling
//
//  Created by Darshan Vikam on 13/08/21.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate {
	@private
	UIWindow *window;
	ViewController *viewController;
	MyView *view;
}
-(BOOL)application: (UIApplication *)application didFinishLaunchingWithOptions: (NSDictionary *)launchOptions {
	// Code
	CGRect winRect = [[UIScreen mainScreen] bounds];
	
	window = [[UIWindow alloc] initWithFrame: winRect];
	viewController = [[ViewController alloc] init];
	view = [[MyView alloc] initWithFrame: winRect];
	
	[window setRootViewController: viewController];
	[viewController setView: view];
	[view release];
	
	[window makeKeyAndVisible];
		
	return YES;
}
-(void)applicationWillResignActive: (UIApplication *)application {
	// Code
}
-(void)applicationDidBecomeActive: (UIApplication *)application {
	// Code
}
-(void)applicationDidEnterBackground: (UIApplication *)application {
	// Code
}
-(void)applicationWillEnterForeground:(UIApplication *)application {
	// Code
}
-(void)applicationWillTerminate: (UIApplication *)application {
	// Code
}
-(void)dealloc {
	// Code
	[view release];
	[viewController release];
	[window release];
	[super dealloc];
}
@end
