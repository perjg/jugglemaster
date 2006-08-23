#import "JMView.h"
#include "jmlib.h"
#import "JMController.h"

@interface JMView(Private)

- (void)drawSiteSwap;
- (void)drawBall:(int)ball;

@end

@implementation JMView

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil)
	{
		[dataSource setFrame:frameRect];
	}
		
	return self;
}

- (void)setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[dataSource setFrame:frameRect];
}

- (BOOL)isFlipped
{
	return YES;
}

- (void)drawRect:(NSRect)rect
{
	int ballNum;
	arm a= [dataSource bodyPartPositions];
	NSBezierPath *headPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(a.hx - a.hr, a.hy - a.hr, a.hr * 2, a.hr * 2)];
	NSBezierPath *rightArmPath = [NSBezierPath bezierPath];
	NSBezierPath *leftArmPath = [NSBezierPath bezierPath];
	
	[headPath setLineWidth:2.0];
	[rightArmPath setLineWidth:2.0];
	[leftArmPath setLineWidth:2.0];
	
	[rightArmPath setLineJoinStyle:NSRoundLineJoinStyle];
	[rightArmPath moveToPoint:NSMakePoint(a.rx[0], a.ry[0])];
	[leftArmPath setLineJoinStyle:NSRoundLineJoinStyle];
	[leftArmPath moveToPoint:NSMakePoint(a.lx[0], a.ly[0])];
	
	for (int i=1;i<=5;i++)
	{
		[rightArmPath lineToPoint:NSMakePoint(a.rx[i], a.ry[i])];
		[leftArmPath lineToPoint:NSMakePoint(a.lx[i], a.ly[i])];
	}
	
	[[NSColor blackColor] set];
	[NSBezierPath fillRect:rect];
	
	[self drawSiteSwap];

	[[NSColor whiteColor] set];
	[headPath stroke];
	[rightArmPath stroke];
	[leftArmPath stroke];
	
	for(ballNum = 0; ballNum < [dataSource numberOfBalls]; ballNum++)
	{
		[self drawBall:ballNum];
	}
}

- (void)drawSiteSwap
{
	NSAttributedString *currentSwap;
	NSShadow *textShadow;
	NSMutableParagraphStyle *paragraphStyle;
	
	if ([dataSource shouldShowPattern])
	{
		textShadow = [[[NSShadow alloc] init] autorelease];
		[textShadow setShadowOffset:NSMakeSize(10,-10)];
		[textShadow setShadowColor:[NSColor darkGrayColor]];
		[textShadow setShadowBlurRadius:1];
		
		paragraphStyle = [[[NSMutableParagraphStyle alloc] init] autorelease];
		[paragraphStyle setAlignment:NSCenterTextAlignment];
		
		currentSwap = [[[NSAttributedString alloc] initWithString:[dataSource currentThrow]
													   attributes:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					[NSFont fontWithName:@"Futura" size:64],
					textShadow,
					[NSColor lightGrayColor],
					paragraphStyle,
					nil]
										forKeys:
				[NSArray arrayWithObjects:
					NSFontAttributeName,
					NSShadowAttributeName,
					NSForegroundColorAttributeName,
					NSParagraphStyleAttributeName,
					nil]]] autorelease];
		
		[currentSwap drawInRect:[self frame]];
	}
}

- (void)drawBall:(int)ball
{
	NSPoint p = [dataSource ballPosition:ball];
	float radius = (float)[dataSource ballDiameter] / 2.0;
	
	NSBezierPath *ballPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(p.x - radius, p.y - radius, radius * 2, radius * 2)];
	
	[[dataSource ballColour:ball] set];
	[ballPath fill];
}

@end
