/*
 * JMTouch - JuggleMaster for iPhone / iPod Touch
 * Version 1.00
 * (C) Per Johan Groland 2009
 *
 * Using JMLib 2.1 (C) Per Johan Groland 2000-2009
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * JMTouch is free software; you can redistribute it and/or modify
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

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "eagl_view.h"

#define USE_DEPTH_BUFFER 1

#include <math.h>

float GetDistance(int x1, int y1, int x2, int y2) {
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end


@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;


// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {

	if ((self = [super initWithCoder:coder])) {
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		
		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		   [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if (!context || ![EAGLContext setCurrentContext:context]) {
			[self release];
			return nil;
		}
		
		animationInterval = 1.0 / 60.0;
	}
  
	site = [[UILabel alloc] initWithFrame:CGRectMake(0, 25, [self bounds].size.width, 25)];
	[site setBackgroundColor:[UIColor colorWithWhite:0.0 alpha:0.5]];
	[site setTextColor:[UIColor whiteColor]];
	[site setFont:[UIFont fontWithName:kFontName size:kLabelFontSize]];
  site.textAlignment = UITextAlignmentCenter;

  // test 2
	overlay = [[UILabel alloc] initWithFrame:CGRectMake(0, 50, [self bounds].size.width, 50)];
	[overlay setBackgroundColor:[UIColor colorWithWhite:0.0 alpha:0.5]];
	[overlay setTextColor:[UIColor redColor]];
	[overlay setFont:[UIFont fontWithName:kFontName size:kLabelFontSize]];
  overlay.textAlignment = UITextAlignmentLeft;

  [self addSubview:site]; 
  [self addSubview:overlay];
  
	return self;
}

- (void)drawSite {
  int start = g_jm->getSiteposStart();
  int stop =  g_jm->getSiteposStop();
  int diff = stop - start;

  NSString* ss = [[NSString alloc] initWithUTF8String:g_jm->getSite()];
  NSString* inactive = [ss substringWithRange:NSMakeRange(0, start)];
  NSString* sub = [ss substringWithRange:NSMakeRange(start,diff)];

  site.text = ss;
  overlay.text = sub;
  
  // calculate proper placement of overlay string
  int full_length = [ss sizeWithFont:[UIFont fontWithName:kFontName size:kLabelFontSize]].width;
  int inactive_length = [inactive sizeWithFont:[UIFont fontWithName:kFontName size:kLabelFontSize]].width;
  int sub_length = [sub sizeWithFont:[UIFont fontWithName:kFontName size:kLabelFontSize]].width;
  
  int start_coord = [self bounds].size.width / 2 - full_length / 2 + inactive_length;

  site.text = ss;
  overlay.text = sub;
  [overlay setFrame:CGRectMake(start_coord, 25, sub_length, 25)];
}

- (void)drawView {
  [self drawSite];
  
  g_jm->doJuggle();
  
	[EAGLContext setCurrentContext:context];
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glViewport(0, 0, backingWidth, backingHeight);  
    
  g_jm->render();

	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
}


- (BOOL)createFramebuffer {
	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if (USE_DEPTH_BUFFER) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}


- (void)destroyFramebuffer {
	
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;
	
	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}


- (void)startAnimation {
	self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
	self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
	[animationTimer invalidate];
	animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
	
	animationInterval = interval;
	if (animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}
}


- (void)dealloc {
	
	[self stopAnimation];
	
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];	
	[super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView:nil];
	CGRect rect = [[UIScreen mainScreen] bounds];

  NSSet* allTouches = [event allTouches];
  
  if ([allTouches count] == 1) // record only the first touch's down coord
	  m_down_point = pt;
	
  g_jm->trackballStart(pt.x, pt.y);
  m_action_on_touch_ended = true;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
  NSSet* allTouches = [event allTouches];
  
  if ([allTouches count] == 1) { // rotate on single touch
    UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
  	//UITouch *touch = [touches anyObject];
	  CGPoint pt = [touch locationInView:nil];
  
    g_jm->setAutoRotate(false);
    g_jm->trackballTrack(pt.x, pt.y);
  }
  else { // move on multitouch
    UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
	  CGPoint pt = [touch locationInView:nil];
    CGRect rect = [[UIScreen mainScreen] bounds];

    
    float dx = -(pt.x - m_down_point.x) * 10.0f / rect.size.width;
    float dy =  (pt.y - m_down_point.y) * 10.0f / rect.size.height;
    
    g_jm->setAutoRotate(false);
    g_jm->move(dx, dy);
    
    m_down_point = pt;
    m_action_on_touch_ended = false;
  }
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
  NSSet* allTouches = [event allTouches];
  UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
  //UITouch *touch = [touches anyObject];
	CGPoint pt = [touch locationInView:nil];
	CGRect rect = [[UIScreen mainScreen] bounds];

	float distance = GetDistance(m_down_point.x, m_down_point.y, pt.x, pt.y);
	printf("Distance between mouse down and up point: %f\n", distance);
  
  if (distance > 7.5f || !m_action_on_touch_ended) return;
  
  if ([touch tapCount] == 1) {
    if ([allTouches count] == 1)
      g_jm->toggleAutoRotate();
    else
      g_jm->togglePause();
  }
  else {
		g_jm->resetCamera();
  }
}

@end
