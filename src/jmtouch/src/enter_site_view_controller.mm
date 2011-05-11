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

#import "enter_site_view_controller.h"

/*@interface EnterSiteViewController
- (void)insertNumber: (int)number;
@end*/

@implementation EnterSiteViewController

@synthesize delegate;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

- (void)updateStyleButtonText {
  char** styles = g_jm->getStyles();

  NSString* styleStr = [[NSString alloc] initWithUTF8String:styles[cur_style]];
  [style setTitle:styleStr forState:UIControlStateNormal];
}

- (void)updateObjectTypeButtonText {
  [objects setTitle:[objectTypes objectAtIndex:cur_object] forState:UIControlStateNormal];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
  cur_object = 0;
  
  objectTypes = [[NSArray alloc] initWithObjects:@"Random", @"Balls", @"Clubs", @"Rings", @"Mixed",nil];
  
  NSString* siteStr = [[NSString alloc] initWithUTF8String:g_jm->getSite()];
  [site setText:siteStr];
  site.backgroundColor = [UIColor greenColor];
  [site setDelegate:self];
  
  // Set style to match current style
  const char* style_str = g_jm->getStyle();
  char** styles = g_jm->getStyles();
  cur_style = 0;
  
  for (int i = 0; i < g_jm->numStyles(); i++) {
    if (strcmp(styles[i], style_str) == 0)
      cur_style = i;
  }
  
  [self updateStyleButtonText];
  
  // Set object type to match current object type
  cur_object = (int)g_jm->getObjectType();
  [self updateObjectTypeButtonText];
  
  // update speed slider
  float cur_speed = g_jm->speed();
  [speed setValue:cur_speed];
  
  [super viewDidLoad];
}

- (IBAction)stylePress {
  if (cur_style < g_jm->numStyles()-1)
    cur_style++;
  else
    cur_style = 0;
  
  [self updateStyleButtonText];
}

- (IBAction)objectsPress {
  if (cur_object < [objectTypes count]-1)
    cur_object++;
  else
    cur_object = 0;
  
  [self updateObjectTypeButtonText];
}

- (bool)verifyCurrentPattern: (bool)changeColor {
  NSString* siteStr = [site text];
  const char* str = [siteStr cStringUsingEncoding:NSASCIIStringEncoding];
  
  bool valid = g_jm->isValidPattern(str);
  
  if (changeColor && valid) {
    site.backgroundColor = [UIColor greenColor];
  }
  else if (changeColor && !valid) {
    site.backgroundColor = [UIColor redColor];    
  }
  
  return valid;
}

- (void)insertNumber: (int)number {
  NSString* siteStr = [[NSString alloc] initWithFormat:@"%@%d", [site text], number];
  [site setText:siteStr];
  [self verifyCurrentPattern:true];
}

- (void)insertChar: (char)ch {
  NSString* siteStr = [[NSString alloc] initWithFormat:@"%@%c", [site text], ch];
  [site setText:siteStr];  
  [self verifyCurrentPattern:true];
}

// Inserts matching paren / bracket
- (void)insertMatching: (char)left withRight: (char)right {
  NSString* current_ns = [site text];
  const char* current = [current_ns cStringUsingEncoding:NSASCIIStringEncoding];
  
  // search backward for opening paren
  if (current) {
    int idx = strlen(current) - 1;
    
    while (idx >= 0) {
      if (current[idx] == left) {
        [self insertChar:right];
        return;
      }
      else if (current[idx] == right) {
        [self insertChar:left];
        return;        
      }
      
      idx--;
    }
  }
  
  [self insertChar:left];
}

- (void)insertParen {
  [self insertMatching:'(' withRight:')'];
}

- (void)insertBracket {
  [self insertMatching:'[' withRight:']'];
}

- (void)backSpace {
  NSString* s = [site text];
  if ([s length] == 0) return;
  [site setText: [s substringToIndex: [s length]-1]];  
  [self verifyCurrentPattern:true];
}

// toggles between letters (for old fashioned cellphone text input)
// fixme: This needs refinement: Add timer, highlight active letter until timer expires
- (void)toggleLetters:(char)from withTo: (char)to {
  NSString* current_ns = [site text];
  const char* current = [current_ns cStringUsingEncoding:NSASCIIStringEncoding];
  
  char last_char = current[ strlen(current)-1 ];
  
  if (last_char >= from && last_char < to) {
    last_char++;
    [self backSpace];
    [self insertChar:last_char];
  }
  else if (last_char == to) {
    last_char = from;
    [self backSpace];
    [self insertChar:last_char];
  }
  else {
    [self insertChar:from];
  }
}

- (IBAction)button0Press {
  [self insertNumber:0];
}

- (IBAction)button1Press {
  [self insertNumber:1];
  //[self toggleLetters:'a' withTo:'c'];
}

- (IBAction)button2Press {
  [self insertNumber:2];
}

- (IBAction)button3Press {
  [self insertNumber:3];
}

- (IBAction)button4Press {
  [self insertNumber:4];
}

- (IBAction)button5Press {
  [self insertNumber:5];
}

- (IBAction)button6Press {
  [self insertNumber:6];
}

- (IBAction)button7Press {
  [self insertNumber:7];
}

- (IBAction)button8Press {
  [self insertNumber:8];
}

- (IBAction)button9Press {
  [self insertNumber:9];
}

- (IBAction)buttonParenPress {
  [self insertParen];
}

- (IBAction)buttonBracketPress {
  [self insertBracket];
}

- (IBAction)buttonCommaPress {
  [self insertChar:','];
}

- (IBAction)buttonXPress {
  [self insertChar:'x'];
}

- (IBAction)buttonBackspacePress {
  [self backSpace];
}

- (IBAction)buttonTogglePress {
  [self insertChar:'T'];  
}

- (IBAction)jugglePress {
  if ([self verifyCurrentPattern:false]) {
    NSString* siteStr = [site text];
    const char* str = [siteStr cStringUsingEncoding:NSASCIIStringEncoding];
    char** styles = g_jm->getStyles();
    
		g_jm->stopJuggle();
		g_jm->setPattern((char*) str);
    g_jm->setStyle(styles[cur_style]);
    g_jm->setObjectType((object_type_t)cur_object);    
    g_jm->setSpeed([speed value]);
		g_jm->startJuggle();
    
    [self.delegate enterSiteViewControllerDidFinish:self];	
  }
}

- (IBAction)cancelPress {
  [self.delegate enterSiteViewControllerDidFinish:self];
}

- (BOOL)shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
  return YES; 
}

// Terminates the editing session
- (BOOL)textFieldShouldReturn:(UITextField*)textField {
	//Terminate editing
  //fixme: causes crash
	//[textField resignFirstResponder];
	
	return YES;
}

- (void)textFieldDidEndEditing:(UITextField*)textField {
  //[self jugglePress];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
