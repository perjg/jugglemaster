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

#import "view_controller.h"
#import "view.h"

@implementation JMViewController

- (id)init
{
	if (self = [super init])
	{
		// make the title of this page the same as the title of this app
		//self.title = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"];
	}
	return self;
}

- (void)dealloc
{
    [jugglerView release];
	[super dealloc];
}

- (void)loadView
{
	// setup the parent content view to host the UITableView
	UIView *contentView = [[UIImageView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	[contentView setBackgroundColor:[UIColor redColor]];
	self.view = contentView;
	[contentView release];
	
	// setup our content view so that it auto-rotates along with the UViewController
	self.view.autoresizesSubviews = YES;
	self.view.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
		
	renderTimer = [[NSTimer scheduledTimerWithTimeInterval:0.02 target:self selector:@selector(doRender:) userInfo:nil repeats:YES] retain];
													
	UINavigationItem *navItem = self.navigationItem;
	
	// Add site button
	UIButton *button = [UIButton buttonWithType:UIButtonTypeNavigation];
	[button setTitle:NSLocalizedString(@"Enter Site", @"") forStates:UIControlStateNormal];
	[button addTarget:self action:@selector(enterSite:) forControlEvents:UIControlEventTouchUpInside];
	navItem.customLeftView = button;

	// Add pattern button
	UIButton *button2 = [UIButton buttonWithType:UIButtonTypeNavigation];
	[button2 setTitle:NSLocalizedString(@"Select Pattern", @"") forStates:UIControlStateNormal];
	[button2 addTarget:self action:@selector(selectPattern:) forControlEvents:UIControlEventTouchUpInside];
	navItem.customTitleView = button2;

	// Add settings button
	UIButton *button3 = [UIButton buttonWithType:UIButtonTypeNavigation];
	[button3 setTitle:NSLocalizedString(@"Settings", @"") forStates:UIControlStateNormal];
	[button3 addTarget:self action:@selector(styleAction:) forControlEvents:UIControlEventTouchUpInside];
	navItem.customRightView = button3;

	
    // Set up content view
	jugglerView = [[[JMView alloc] initWithFrame:self.view.bounds] autorelease];
	[jugglerView setBackgroundColor:[UIColor lightGrayColor]];
	[self.view addSubview: jugglerView];
	
	jm = JMLib::alloc();
	
	jm->setWindowSize([jugglerView frame].size.width, [jugglerView frame].size.height);
	jm->setSpeed(1.0);
	jm->setScalingMethod(SCALING_METHOD_DYNAMIC);
	//jm->setPatternDefault();
	//jm->setStyleDefault();
	jm->setPattern("534");
	jm->startJuggle();
	jm->doJuggle();

	[jugglerView setJMLib:jm];
}

- (void)enterSite:(id)sender
{
	UIActionSheet *styleAlert =
		[[UIActionSheet alloc] initWithTitle:@"Enter site:" message:@""
								delegate:self defaultButton:nil cancelButton:nil
								otherButtons:@"534", @"Mills' Mess", @"633", @"Cancel", nil];
	[styleAlert showInView:self.view];
	[styleAlert release];
}

- (void)selectPattern:(id)sender
{
	UIActionSheet *styleAlert =
		[[UIActionSheet alloc] initWithTitle:@"Select pattern:" message:@""
								delegate:self defaultButton:nil cancelButton:nil
								otherButtons:@"OK", @"Cancel", nil];
	[styleAlert showInView:self.view];
	[styleAlert release];
}

- (void)styleAction:(id)sender
{
	UIActionSheet *styleAlert =
		[[UIActionSheet alloc] initWithTitle:@"UIBarStyle" message:@"Select pattern:"
								delegate:self defaultButton:nil cancelButton:nil
								otherButtons:@"Default", @"BlackOpaque", @"BlackTranslucent", @"Cancel", nil];
	[styleAlert showInView:self.view];
	[styleAlert release];
}

/*
- (void)modalView:(UIModalView *)modalView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch (buttonIndex)
	{
		case 0:
			self.navigationController.navigationBarStyle = UIBarStyleDefault;
			break;
		case 1:
			self.navigationController.navigationBarStyle = UIBarStyleBlackOpaque;
			break;
		case 2:
			self.navigationController.navigationBarStyle = UIBarStyleBlackTranslucent;
			break;
	}
}
*/

- (void)doRender:(NSTimer*) timer
{
/*
  port this to jmlib->doJuggle:
  
  - figure out the natural speed for calling doJuggle
  - call doJuggle at the fastest speed possible
  - doJuggle will move pattern only when a specific time
    interval has elapsed
    (calling several times in a row until the time since last frame is ok)

    if (FramesSinceSync >=  1 * (int) CurrentFrameRate)
    {
        struct timeval tvnow;
        unsigned now;
            
        # ifdef GETTIMEOFDAY_TWO_ARGS
            struct timezone tzp;
            gettimeofday(&tvnow, &tzp);
        # else
            gettimeofday(&tvnow);
        # endif
        
        now = (unsigned) (tvnow.tv_sec * 1000000 + tvnow.tv_usec);
        if (FramesSinceSync == 0)
        {
            LastSyncTime = now;
        }
        else
        {
            unsigned Delta = now - LastSyncTime;
            if (Delta > 20000)
            {
                LastSyncTime = now;
                CurrentFrameRate = 
                    (FramesSinceSync * 1.0e6f) / Delta;
                FramesSinceSync = 0;
            }
        }
    }
    
    FramesSinceSync++;
    
    if (state.Time > 150.0f)
    {
        UpdatePattern(&state, MinObjects, MaxObjects, 
            MinHeightInc, MaxHeightInc);
    }
    DrawGLScene(&state);
    
    if (CurrentFrameRate > 1.0e-6f)
    {
        state.Time += JuggleSpeed / CurrentFrameRate;
        state.SpinAngle += SpinSpeed / CurrentFrameRate;
        state.TranslateAngle += 
            TranslateSpeed / CurrentFrameRate;
    }
*/

	if (jm) jm->doJuggle();
	[jugglerView setNeedsDisplay];
}

#pragma mark UIViewController delegates

- (void)viewWillAppear:(BOOL)animated
{
/*
	NSIndexPath *tableSelection = [myTableView indexPathForSelectedRow];
	[myTableView deselectRowAtIndexPath:tableSelection animated:NO];
*/
}

- (void)viewDidAppear:(BOOL)animated
{
	// do something here as our view re-appears
}

@end

