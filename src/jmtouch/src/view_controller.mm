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
  /*
	jugglerView = [[[JMView alloc] initWithFrame:self.view.bounds] autorelease];
	[jugglerView setBackgroundColor:[UIColor lightGrayColor]];
	[self.view addSubview: jugglerView];
  */
	glView = [[EAGLTrackBallView alloc] initWithFrame:self.view.bounds pixelFormat:kEAGLPixelFormat_RGBA8888_D24];
	[self.view addSubview: glView];
	  
  jm = JMLib::alloc();
  //jm = JMLib::alloc_JuggleMaster();
  //jm = JMLib::alloc_JuggleSaver();
	jm->setWindowSize([jugglerView frame].size.width, [jugglerView frame].size.height);
  jm->setPatternDefault();
  jm->setStyleDefault();
  jm->setScalingMethod(SCALING_METHOD_DYNAMIC);
  jm->startJuggle();

  jm->initialize();

	[jugglerView setJMLib:jm];
}

- (void)enterSite:(id)sender
{
	UIActionSheet *styleAlert =
		[[UIActionSheet alloc] initWithTitle:@"Enter site:" message:@""
								delegate:self defaultButton:nil cancelButton:nil
								otherButtons:@"534 Mill's Mess", @"JuggleSaver 1", @"JuggleSaver 2", @"Flat", @"Cancel", nil];
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
		[[UIActionSheet alloc] initWithTitle:@"Choose a UIBarStyle:"
								delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil
								otherButtonTitles:@"Default", @"BlackOpaque", @"BlackTranslucent", @"Cancel", nil];
	[styleAlert showInView:self.view];
	[styleAlert release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
  jm->stopJuggle();
  
	switch (buttonIndex)
	{
		case 0: // 534 Mill's Mess (JuggleMaster)
      jm->setRenderingMode(RENDERING_OPENGL_3D);
      jm->setPattern("534");
      jm->setStyle("Mills Mess");
 			//self.navigationController.navigationBar.barStyle = UIBarStyleDefault;
			break;
		case 1: // JuggleSaver 1
      jm->setRenderingMode(RENDERING_OPENGL_3D);
      jm->setPattern("6c3b3r");
			//self.navigationController.navigationBar.barStyle = UIBarStyleBlackOpaque;
			break;
		case 2: // JuggleSaver 2
      jm->setRenderingMode(RENDERING_OPENGL_3D);
      jm->setPattern("7c@(-2.6,0.5,-90,0)>(3,0.5,90,0)*1 3c@(2,0.5,90,0)>(-2,0.5,-90,0)*1");
			//self.navigationController.navigationBar.barStyle = UIBarStyleBlackTranslucent;
			break;
    case 3:
      jm->setRenderingMode(RENDERING_OPENGL_2D);
      jm->setPattern("633");
      jm->setStyle("Reverse");
      break;
	}
  
  jm->startJuggle();
}

- (void)doRender:(NSTimer*) timer
{
	if (jm) jm->doJuggle();
  if (jm) jm->render();
  [glView setEAGLMatrix];
 	[glView swapBuffers];
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

