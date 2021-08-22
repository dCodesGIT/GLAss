//
//  Window.m
//  
//  Created by Darshan Vikam on 28/07/21.
//

// Importing required headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

// Declaring interfaces
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface MyView : NSView
@end

// Entry point function - main()
int main(int argc, char* argv[]) {
	// Code
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSApp = [NSApplication sharedApplication];		// NSApp is global variable given by MacOS
	[NSApp setDelegate : [[AppDelegate alloc] init]];
	
	[NSApp run];            // Run loop or message loop or game loop
	
	[pool release];
	
	return 0;
}

// Implementation of AppDelegate interface
@implementation AppDelegate {
	@private
	NSWindow *window;
	MyView *view;
}
-(void)applicationDidFinishLaunching : (NSNotification *)aNotification {
	// Code
	NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
	window = [[NSWindow alloc] initWithContentRect : win_rect
			styleMask : NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
			backing : NSBackingStoreBuffered
			defer : NO];
	[window setTitle : @"MacOS Window"];
	[window center];
	
	view = [[MyView alloc] initWithFrame : win_rect];
	
	[window setContentView : view];
	[window setDelegate : self];
	[window makeKeyAndOrderFront : self];
}
-(void)applicationWillTerminate : (NSNotification *)aNotification {
	// Code
}
-(void)windowWillClose : (NSNotification *)aNotification {
	// Code
	[NSApp terminate : self];
}
-(void)dealloc {
	// Code
	[view release];
	[window release];
	[super dealloc];
}
@end

// Implementation of MyView interface
@implementation MyView {
	@private
	NSString *text;
}
-(id)initWithFrame : (NSRect)frame {
	// Code
	self = [super initWithFrame : frame];
	if(self) {
		text = @"Hello Mac!!!";
	}
	return self;
}
-(void)drawRect : (NSRect)dirtyRect {
	// Code
	NSColor *bgColor = [NSColor blackColor];
	[bgColor set];
	NSRectFill(dirtyRect);
	
	NSDictionary *textDictionary = [[NSDictionary alloc] initWithObjectsAndKeys :
				       [NSFont fontWithName : @"Helvetica" size : 32], NSFontAttributeName,
				       [NSColor greenColor], NSForegroundColorAttributeName,
				       nil];
	NSSize textSize = [text sizeWithAttributes : textDictionary];
	
	NSPoint point;
	point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
	point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;		// additional 12 pixels due to title bar
	
	[text drawAtPoint : point withAttributes : textDictionary];
}
-(BOOL)acceptsFirstResponder {
	// Code
	[[self window] makeFirstResponder : nil];
	return YES;
}
-(void)keyDown : (NSEvent *)theEvent {
	// Code
	int key = [[theEvent characters] characterAtIndex : 0];
	switch(key) {
		case 27 :		// ESC
			[self release];
			[NSApp terminate : self];
			break;
		case 'F' :
		case 'f' :
			[[self window] toggleFullScreen : self];
			break;
	}
}
-(void)mouseDown : (NSEvent *)theEvent {
	// Code
	text = @"Left mouse button clicked";
	[self setNeedsDisplay : YES];
}
-(void)rightMouseDown : (NSEvent *)theEvent {
	// Code
	text = @"Right mouse button clicked";
	[self setNeedsDisplay : YES];
}
-(void)otherMouseDown : (NSEvent *)theEvent {
	// Code
	text = @"Hello Mac !!!";
	[self setNeedsDisplay : YES];
}
-(void)dealloc {
	// Code
	[super dealloc];
}
@end
