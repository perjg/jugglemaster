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

#import "app_delegate.h"
#import "main_view_controller.h"
#import "eagl_view.h"

@implementation jmtouchAppDelegate

@synthesize window;
@synthesize mainViewController;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
  /*
   UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:viewController];
   // Override point for customization after app launch
   [window addSubview:navController.view];
   [window makeKeyAndVisible];   
   */
  
  
	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	//MainViewController *aController = [[MainViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
  mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	//[window addSubview:[mainViewController view]];
  //[window makeKeyAndVisible];  
  
  jm = JMLib::alloc();
  //jm = JMLib::alloc_JuggleMaster();
  //jm = JMLib::alloc_JuggleSaver();
  jm->initialize();

	jm->setWindowSize([glView frame].size.width, [glView frame].size.height);
  jm->setPatternDefault();
  jm->setStyleDefault();
  jm->setScalingMethod(SCALING_METHOD_DYNAMIC);
  jm->startJuggle();

	[glView setJMLib:jm];
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}

- (IBAction)showInfo {
  //jm->setRenderingMode(RENDERING_OPENGL_2D);
  //jm->setPattern("3B@(1,-0.4)>(2,4.2)/(-2,1)3B@(-1.8,4.4)>(-2.1,0)");
  [glView stopAnimation];
  [glView removeFromSuperview];		
  
  [mainViewController setAppDelegate:self];
  [mainViewController setJm:jm];
  [window addSubview:[mainViewController view]];
  [window makeKeyAndVisible];
}

- (void)showJuggler {
  [mainViewController.view removeFromSuperview];
  [window addSubview:glView];

	glView.animationInterval = 1.0 / 60.0;
  [glView startAnimation];
}

- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}

- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
