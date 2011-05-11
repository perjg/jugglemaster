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

#import <UIKit/UIKit.h>
#include "jmlib.h"

extern JMLib* g_jm;

@protocol EnterSiteViewControllerDelegate;

@interface EnterSiteViewController : UIViewController <UITextFieldDelegate> {
	id <EnterSiteViewControllerDelegate> delegate;
  IBOutlet UITextField* site;
  IBOutlet UIButton* style;
  IBOutlet UIButton* objects;
  IBOutlet UISlider* speed;
  
  @private
    int cur_object;
    NSArray* objectTypes;
    int cur_style;
}

@property (nonatomic, assign) id <EnterSiteViewControllerDelegate> delegate;
- (IBAction)stylePress;
- (IBAction)objectsPress;
- (IBAction)jugglePress;
- (IBAction)cancelPress;

// Number keypad buttons
- (IBAction)button0Press;
- (IBAction)button1Press;
- (IBAction)button2Press;
- (IBAction)button3Press;
- (IBAction)button4Press;
- (IBAction)button5Press;
- (IBAction)button6Press;
- (IBAction)button7Press;
- (IBAction)button8Press;
- (IBAction)button9Press;
- (IBAction)buttonParenPress;
- (IBAction)buttonBracketPress;
- (IBAction)buttonCommaPress;
- (IBAction)buttonXPress;
- (IBAction)buttonTogglePress;
- (IBAction)buttonBackspacePress;
@end

@protocol EnterSiteViewControllerDelegate
- (void)enterSiteViewControllerDidFinish:(EnterSiteViewController *)controller;
@end


