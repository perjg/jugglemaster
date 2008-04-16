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
	//return;
	if (!jm) return;

	CGRect bounds = [self bounds];
	CGContext* context = UIGraphicsGetCurrentContext(); //CGLayerGetContext(_cgLayer);
	CGContextSetRGBStrokeColor(context, 0, 0, 0, 1);
	//CGContextTranslateCTM(context, bounds.size.width/4, bounds.size.height/4);
	//CGContextRotateCTM(context, PI*0.1);
	
	bool globallMode = false;
	arm* ap = &(jm->ap);
	ball* rhand = &(jm->rhand);
	ball* lhand = &(jm->lhand);
	hand* handp = &(jm->handpoly);

	if (!globallMode) {
		CGContextBeginPath(context);
		CGContextAddArc(context, ap->hx, ap->hy, ap->hr, 0, 2*PI, true);
		CGContextStrokePath(context);
		
		// right arm
		CGContextBeginPath(context);
		for(int i=0; i<5; i++) {
			CGContextMoveToPoint(context, ap->rx[i], ap->ry[i]);
			CGContextAddLineToPoint(context, ap->rx[i+1], ap->ry[i+1]);
		}
		CGContextStrokePath(context);

		// left arm
		CGContextBeginPath(context);
		for(int i=0; i<5; i++) {
			CGContextMoveToPoint(context, ap->lx[i], ap->ly[i]);
			CGContextAddLineToPoint(context, ap->lx[i+1], ap->ly[i+1]);
		}
		CGContextStrokePath(context);

		// right hand
		CGContextBeginPath(context);
		CGContextMoveToPoint(context, rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);

		for (int i=1; i<10; i++) {
			CGContextAddLineToPoint(context, rhand->gx + handp->rx[i], rhand->gy + handp->ry[i]);
		}
		CGContextAddLineToPoint(context, rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);
		CGContextStrokePath(context);

		// left hand
		CGContextBeginPath(context);
		CGContextMoveToPoint(context, lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);

		for (int i=1; i<10; i++) {
			CGContextAddLineToPoint(context, lhand->gx + handp->lx[i], lhand->gy + handp->ly[i]);
		}
		CGContextAddLineToPoint(context, lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);
		CGContextStrokePath(context);
	}

	//CGColorRef color = 

	// draw balls
	for(int i=jm->numBalls()-1;i>=0;i--) {
		CGContextBeginPath(context);

		CGContextSetRGBFillColor(context, 255, 0, 0, 1.0);

		int rad = jm->getBallRadius();
		if (rad < 2) rad = 1;

		CGContextAddArc(context, jm->b[i].gx+rad, jm->b[i].gy+rad, rad, 0, 2*PI, true);
		CGContextFillPath(context);
		CGContextStrokePath(context);
	}
}

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

- (void)setJMLib:(JMLib*)jm_
{
	jm = jm_;
}

@end
