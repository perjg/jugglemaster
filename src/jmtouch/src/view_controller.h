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

#import <UIKit/UIKit.h>
#include "jmlib.h"
#include "validator.h"

#import "EAGLTrackBallView.h"

@class JMView;

@interface JMViewController : UIViewController <UINavigationBarDelegate, UIActionSheetDelegate>
{
  JMView* jugglerView;
 	EAGLTrackBallView* glView;

	@private
		JMLib* jm;
		NSTimer *renderTimer;
}

@end
