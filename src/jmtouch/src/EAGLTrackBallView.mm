/*

===== IMPORTANT =====

This is sample code demonstrating API, technology or techniques in development.
Although this sample code has been reviewed for technical accuracy, it is not
final. Apple is supplying this information to help you plan for the adoption of
the technologies and programming interfaces described herein. This information
is subject to change, and software implemented based on this sample code should
be tested with final operating system software and final documentation. Newer
versions of this sample code may be provided with future seeds of the API or
technology. For information about updates to this and other developer
documentation, view the New & Updated sidebars in subsequent documentation
seeds.

=====================

File: EAGLTrackBallView.m
Abstract: Convenience subclass of EAGLView that puts the 3D world into a virtual
trackball.

Version: 1.3

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/

#define __DELEGATE_IVAR__ _trackingDelegate
#define __DELEGATE_METHODS_IVAR__ _trackingDelegateMethods

#import "EAGLTrackBallView.h"
#import "OpenGL_Internal.h"

//FUNCTIONS:

static inline GLfloat _VectorLength(const GLfloat v[3])
{
	return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static inline GLfloat _VectorDotProduct(const GLfloat v1[3], const GLfloat v2[3])
{
	return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

static inline void _VectorCrossProduct(const GLfloat v1[3], const GLfloat v2[3], GLfloat res[3])
{
	res[0] = v1[1] * v2[2] - v1[2] * v2[1];
	res[1] = v1[2] * v2[0] - v1[0] * v2[2];
	res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static inline void _QuaternionMultiply(const GLfloat q1[4], const GLfloat q2[4], GLfloat res[4])
{
	res[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
	res[1] = q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2];
	res[2] = q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0];
	res[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];
}

static inline void _QuaternionSetFromRotationAxisAndAngle(GLfloat q[4], const GLfloat axis[3], GLfloat angle)
{
	GLfloat		invLength = 1.0 / _VectorLength(axis);
	GLfloat		halfAngle = DEGREES_TO_RADIANS(angle) / 2.0;
	GLfloat		sinHalfAngle = sinf(halfAngle);
	
	q[0] = axis[0] * sinHalfAngle * invLength;
	q[1] = axis[1] * sinHalfAngle * invLength;
	q[2] = axis[2] * sinHalfAngle * invLength;
	q[3] = cosf(halfAngle);
}

static inline void _QuaternionToMatrix(const GLfloat q[4], GLfloat m[16])
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = q[0] + q[0]; y2 = q[1] + q[1]; z2 = q[2] + q[2];
	xx = q[0] * x2; xy = q[0] * y2; xz = q[0] * z2;
	yy = q[1] * y2; yz = q[1] * z2; zz = q[2] * z2;
	wx = q[3] * x2; wy = q[3] * y2; wz = q[3] * z2;

	m[0] = 1.0 - (yy + zz);
	m[4] = xy - wz;
	m[8] = xz + wy;
	
	m[1] = xy + wz;
	m[5] = 1.0 - (xx + zz);
	m[9] = yz - wx;
	
	m[2] = xz - wy;
	m[6] = yz + wx;
	m[10] = 1.0 - (xx + yy);
	
	m[3] = 0.0;
	m[7] = 0.0;
	m[11] = 0.0;
	
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
}

static void _DictionaryApplierFunction(const void* key, const void* value, void* context)
{
	*((CGPoint*)[(NSMutableData*)value mutableBytes]) = [(UITouch*)key locationInView:(UIView*)context];
}

//CLASS IMPLEMENTATIONS:

@implementation EAGLTrackBallView

@synthesize trackingDelegate=_trackingDelegate;
@synthesize jm;

- (id) initWithFrame:(CGRect)frame pixelFormat:(EAGLPixelFormat)format
{
	if((self = [super initWithFrame:frame pixelFormat:format])) {
		[self setMultipleTouchEnabled:YES];
		
		_activeTouches = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		[self reset];
	}
	
	return self;
}

- (void) dealloc
{
	if(_activeTouches)
	CFRelease(_activeTouches);
	
	[super dealloc];
}

- (void) setTrackingDelegate:(id<EAGLTrackBallViewDelegate>)delegate
{
	_trackingDelegate = delegate;
	
	SET_DELEGATE_METHOD_BIT(16, didBeginTrackingInTrackBallView:withEvent:);
	SET_DELEGATE_METHOD_BIT(17, didUpdateTrackingInTrackBallView:withEvent:);
	SET_DELEGATE_METHOD_BIT(18, didEndTrackingInTrackBallView:withEvent:);
}

- (void) reset
{
	if(CFDictionaryGetCount(_activeTouches))
	return;
	
	_translation[0] = _translation[1] = _translation[2] = 0.0;
	_orientation[0] = _orientation[1] = _orientation[2] = 0.0; _orientation[3] = 1.0;
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	CFIndex				count = CFDictionaryGetCount(_activeTouches);
	UITouch*			touch;
	
	if([touches count] >= 3)
	[self reset];
	
	for(touch in touches)
	CFDictionarySetValue(_activeTouches, touch, [NSMutableData dataWithLength:sizeof(CGPoint)]);
	
	CFDictionaryApplyFunction(_activeTouches, _DictionaryApplierFunction, self);
	_startTranslation[0] = _translation[0]; _startTranslation[1] = _translation[1]; _startTranslation[2] = _translation[2];
	_startOrientation[0] = _orientation[0]; _startOrientation[1] = _orientation[1]; _startOrientation[2] = _orientation[2]; _startOrientation[3] = _orientation[3];
	
	if(!count)
	[_trackingDelegate didBeginTrackingInTrackBallView:self withEvent:event];
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	CGSize				size = [self bounds].size;
	float				scale = size.width,
						ratio = size.width / size.height,
						sensibility = 1.0;
	UITouch*			touchList[2];
	NSMutableData*		pointList[2];
	CGPoint				startPosition,
						position;
	float				startDistance,
						distance,
						radius;
	GLfloat				startVector[3],
						vector[3],
						axis[3],
						ls,
						le,
						lp,
						cosAngle,
						sinAngle,
						angle,
						dq[4];
						
	switch(CFDictionaryGetCount(_activeTouches)) {
		
		case 1:
		CFDictionaryGetKeysAndValues(_activeTouches, (const void**)touchList, (const void**)pointList);
		startPosition = *((CGPoint*)[pointList[0] mutableBytes]);
		position = [touchList[0] locationInView:self];
		
		if(ratio > 1.0)
		radius = 0.5 / ratio;
		else
		radius = 0.5;
		
		startVector[0] = sensibility * (startPosition.x / size.width - 0.5);
		startVector[1] = sensibility * ((1.0 - startPosition.y / size.height) - 0.5);
		if(startVector[0] * startVector[0] + startVector[1] * startVector[1] > radius * radius)
		startVector[2] = 0.0;
		else
		startVector[2] = sqrtf(radius * radius - startVector[0] * startVector[0] - startVector[1] * startVector[1]);
		
		vector[0] = sensibility * (position.x / size.width - 0.5);
		vector[1] = sensibility * ((1.0 - position.y / size.height) - 0.5);
		if(vector[0] * vector[0] + vector[1] * vector[1] > radius * radius)
		vector[2] = 0.0;
		else
		vector[2] = sqrtf(radius * radius - vector[0] * vector[0] - vector[1] * vector[1]);
		
		_VectorCrossProduct(startVector, vector, axis);
		ls = 1.0 / _VectorLength(startVector);
		le = 1.0 / _VectorLength(vector);
		lp = 1.0 / _VectorLength(axis);
		cosAngle = _VectorDotProduct(startVector, vector) * ls * le;
		sinAngle = ls * le / lp;
		
		axis[0] *= lp;
		axis[1] *= lp;
		axis[2] *= lp;
		angle = sensibility * RADIANS_TO_DEGREES(atan2f(sinAngle, cosAngle));
		_QuaternionSetFromRotationAxisAndAngle(dq, axis, angle);
		_QuaternionMultiply(dq, _startOrientation, _orientation);
		break;
		
		case 2:
		CFDictionaryGetKeysAndValues(_activeTouches, (const void**)touchList, (const void**)pointList);
		startPosition.x = (((CGPoint*)[pointList[0] mutableBytes])->x + ((CGPoint*)[pointList[1] mutableBytes])->x) / 2.0;
		startPosition.y = (((CGPoint*)[pointList[0] mutableBytes])->y + ((CGPoint*)[pointList[1] mutableBytes])->y) / 2.0;
		startDistance = sqrtf((((CGPoint*)[pointList[1] mutableBytes])->x - ((CGPoint*)[pointList[0] mutableBytes])->x) * (((CGPoint*)[pointList[1] mutableBytes])->x - ((CGPoint*)[pointList[0] mutableBytes])->x) + (((CGPoint*)[pointList[1] mutableBytes])->y - ((CGPoint*)[pointList[0] mutableBytes])->y) * (((CGPoint*)[pointList[1] mutableBytes])->y - ((CGPoint*)[pointList[0] mutableBytes])->y));
		position.x = ([touchList[0] locationInView:self].x + [touchList[1] locationInView:self].x) / 2.0;
		position.y = ([touchList[0] locationInView:self].y + [touchList[1] locationInView:self].y) / 2.0;
		distance = sqrtf(([touchList[1] locationInView:self].x - [touchList[0] locationInView:self].x) * ([touchList[1] locationInView:self].x - [touchList[0] locationInView:self].x) + ([touchList[1] locationInView:self].y - [touchList[0] locationInView:self].y) * ([touchList[1] locationInView:self].y - [touchList[0] locationInView:self].y));
		
		_translation[0] = _startTranslation[0] + (position.x - startPosition.x) / scale;
		_translation[1] = _startTranslation[1] - (position.y - startPosition.y) / scale;
		_translation[2] = _startTranslation[2] + (distance - startDistance) / scale;
		break;
		
	}
	
	[_trackingDelegate didUpdateTrackingInTrackBallView:self withEvent:event];
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch*			touch;
	
	for(touch in touches)
	CFDictionaryRemoveValue(_activeTouches, touch);
	
	if(!CFDictionaryGetCount(_activeTouches))
	[_trackingDelegate didEndTrackingInTrackBallView:self withEvent:event];
}

- (void) setEAGLMatrix
{
	GLfloat				matrix[16];
	
	glTranslatef(_translation[0], _translation[1], _translation[2]);
	
	_QuaternionToMatrix(_orientation, matrix);
	glMultMatrixf(matrix);
}

- (BOOL) isTracking
{
	return (CFDictionaryGetCount(_activeTouches) ? YES : NO);
}

- (void)setJMLib:(JMLib*)jm_
{
	jm = jm_;
  //jm->initialize();
}

@end
