//
//  MyView.m
//  Window
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
	}
	return self;
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
