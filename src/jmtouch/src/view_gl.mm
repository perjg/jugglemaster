/*
 * JMTouch - JuggleMaster for iPhone / iPod Touch
 * Version 1.00
 * (C) Per Johan Groland 2008
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

#import "view.h"

#define kBrushSize		25.0
#define kBrushAlpha		0.5

@implementation JMView

@synthesize jm;

- (id) initWithFrame:(CGRect)frame
{
	if((self = [super initWithFrame:frame])) {
	}
	
	return self;
}

- (void) dealloc
{
	CGLayerRelease(_cgLayer);
	CGContextRelease(_cgContext);
	
	[super dealloc];
}

- (void) drawRect:(CGRect)clip
{
	if (!jm) return;

}

/*
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	//UITouch*			touch = [touches anyObject];
	
	if (jm) jm->togglePause();
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	//UITouch*			touch = [touches anyObject];
	//CGPoint				location = [touch locationInView],
	//					lastLocation = [touch previousLocationInView];
	//CGContextRef		context = CGLayerGetContext(_cgLayer);
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	//CGRect				bounds = [self bounds];
}
*/

- (void)setJMLib:(JMLib*)jm_
{
	jm = jm_;
}

@end
