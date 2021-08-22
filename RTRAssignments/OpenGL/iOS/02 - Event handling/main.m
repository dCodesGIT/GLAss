//
//  main.m
//  Event handling
//
//  Created by Darshan Vikam on 13/08/21.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char* argv[]) {
	// Code
	NSString *appDelegateClassName;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	appDelegateClassName = NSStringFromClass([AppDelegate class]);

	int ret = UIApplicationMain(argc, argv, nil, appDelegateClassName);

	[pool release];
	
	return ret;
}
