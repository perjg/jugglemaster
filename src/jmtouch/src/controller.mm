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

#import "controller.h"
#import "view_controller.h"
#import "view.h"

@implementation JMController

@synthesize window;

// Convenience macro for accessing the application delegate, cast to the appropriate class.
#define _AppDelegate_ ((AppDelegate*)[[UIApplication sharedApplication] delegate])

- (void)applicationDidFinishLaunching:(UIApplication*) application {	
	// Create window
    //self.window = [[[NSBundle mainBundle] loadNibNamed:@"jmtouch" owner:self options:nil] objectAtIndex:0];
	self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    //self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [window setBackgroundColor:[UIColor lightGrayColor]];
	
	// Set up main view navigation controller
  JMViewController *navController = [[JMViewController alloc] init];
	navigationController = [[UINavigationController alloc] initWithRootViewController:navController];
	navigationController.navigationBar.barStyle = UIBarStyleDefault;
	[navController release];
	
	// Add the navigation controller's view to the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}

- (void)dealloc {
	[navigationController release];   
	[window release];
	[super dealloc];
}

@end
