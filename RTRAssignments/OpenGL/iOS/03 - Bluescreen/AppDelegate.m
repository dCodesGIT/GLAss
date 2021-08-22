//
//  AppDelegate.m
//  Bluescreen
//
//  Created by Darshan Vikam on 13/08/21.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "OpenGLESView.h"

@implementation AppDelegate {
	@private
	UIWindow *window;
	ViewController *viewController;
	OpenGLESView *view;
}
-(BOOL)application: (UIApplication *)application didFinishLaunchingWithOptions: (NSDictionary *)launchOptions {
	// Code
	CGRect winRect = [[UIScreen mainScreen] bounds];
	
	window = [[UIWindow alloc] initWithFrame: winRect];
	viewController = [[ViewController alloc] init];
	view = [[OpenGLESView alloc] initWithFrame: winRect];
	
	[window setRootViewController: viewController];
	[viewController setView: view];
	[view release];
	
	[view startAnimation];

	[window makeKeyAndVisible];
		
	return YES;
}
-(void)applicationWillResignActive: (UIApplication *)application {
	// Code
	[view stopAnimation];
}
-(void)applicationDidBecomeActive: (UIApplication *)application {
	// Code
	[view startAnimation];
}
-(void)applicationDidEnterBackground: (UIApplication *)application {
	// Code
}
-(void)applicationWillEnterForeground:(UIApplication *)application {
	// Code
}
-(void)applicationWillTerminate: (UIApplication *)application {
	// Code
	[view stopAnimation];
}
-(void)dealloc {
	// Code
	[view release];
	[viewController release];
	[window release];
	[super dealloc];
}
@end
