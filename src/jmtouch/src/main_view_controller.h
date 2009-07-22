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

#import "settings_view_controller.h"
#import "enter_site_view_controller.h"
#include "jmlib.h"
#include "sqlite_patterns.h"

extern JMPatterns* g_pattern_lib;

@class jmtouchAppDelegate;
 
//@interface MainViewController : UINavigationController <FlipsideViewControllerDelegate> {
@interface MainViewController : UIViewController <FlipsideViewControllerDelegate, EnterSiteViewControllerDelegate> {
  IBOutlet UITableView*  table;
  IBOutlet UISearchBar* search;
  
  @private
    NSArray* tableItems;
  
    NSMutableArray* categories;
    NSMutableArray* patterns_in_category;
    NSArray* fakeItems;
  
    jmtouchAppDelegate* appDelegate;
    JMLib* jm;
}

@property JMLib* jm;
- (IBAction)done;
- (void)setAppDelegate:(jmtouchAppDelegate*)appDelegate_;
//- (void)setJMLib:(JMLib*)jm_;

@end
