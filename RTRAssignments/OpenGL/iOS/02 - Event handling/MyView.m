//
//  MyView.m
//  Event handling
//
//  Created by Darshan Vikam on 13/08/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "MyView.h"

@implementation MyView {
	@private
	NSString *text;
}
-(id)initWithFrame: (CGRect)frame {
	// Code
	self = [super initWithFrame : frame];
	
	if(self) {
		text = @"Hello iOS !!!";
		
		// Gesture
		UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onSingleTap:)];
		[singleTap setNumberOfTapsRequired: 1];
		[singleTap setNumberOfTouchesRequired: 1];
		[singleTap setDelegate: self];
		[self addGestureRecognizer: singleTap];
		
		UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onDoubleTap:)];
		[doubleTap setNumberOfTapsRequired: 2];
		[doubleTap setNumberOfTouchesRequired: 1];
		[doubleTap setDelegate: self];
		[self addGestureRecognizer: doubleTap];
		
		[singleTap requireGestureRecognizerToFail: doubleTap];
		
		UISwipeGestureRecognizer *swipe = [[UISwipeGestureRecognizer alloc] initWithTarget: self action: @selector(onSwipe:)];
		[swipe setNumberOfTouchesRequired: 1];
		[swipe setDelegate: self];
		[self addGestureRecognizer: swipe];
		
		UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget: self action: @selector(onLongPress:)];
		[longPress setNumberOfTouchesRequired: 1];
		[longPress setDelegate: self];
		[self addGestureRecognizer: longPress];
	}
	return self;
}
-(void)onSingleTap: (UITapGestureRecognizer *)gesture {
	// Code
	text = @"Single Tap";
	[self setNeedsDisplay];
}
-(void)onDoubleTap: (UITapGestureRecognizer *)gesture {
	// Code
	text = @"Double Tap";
	[self setNeedsDisplay];
}
-(void)onLongPress: (UILongPressGestureRecognizer *)gesture {
	// Code
	text = @"Long Press";
	[self setNeedsDisplay];
}
-(void)onSwipe: (UISwipeGestureRecognizer *)gesture {
	// Code
/*	text = @"Swipe";
	[self setNeedsDisplay];
*/
	[self release];
	exit(0);
}
// Only for immediate mode rendering
-(void)drawRect: (CGRect)rect {
	// Drawing code
	UIColor *bgColor = [UIColor blackColor];
	[bgColor set];
	UIRectFill(rect);
	
	NSDictionary *textDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
					[UIFont fontWithName: @"Helvetica" size: 32], NSFontAttributeName,
					[UIColor greenColor], NSForegroundColorAttributeName,
					nil];
	CGSize textSize = [text sizeWithAttributes: textDictionary];
	
	CGPoint point;
	point.x = (rect.size.width / 2) - (textSize.width / 2);
	point.y = (rect.size.height / 2) - (textSize.height / 2);
	
	[text drawAtPoint: point withAttributes: textDictionary];
}
-(void)dealloc {
	// Code
	[super dealloc];
}
@end
