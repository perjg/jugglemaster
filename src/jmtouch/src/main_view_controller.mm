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

#import "main_view_controller.h"
#import "main_view.h"
#import "app_delegate.h"

extern JMPatterns* g_pattern_lib;

@implementation MainViewController

@synthesize jm;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}


 // Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
  tableItems = [[NSArray alloc] initWithObjects:@"Enter Siteswap",@"Settings",@"Random Pattern",@"Set 2D Mode",@"About",nil];

  // Open pattern database
  NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"patterns.db"];    
  g_pattern_lib = new JMPatterns();
  g_pattern_lib->loadDatabase([path cStringUsingEncoding:NSASCIIStringEncoding]);
  
  // Load categories
  categories = [[NSMutableArray alloc] init];
  categoriesIndex = [[NSMutableArray alloc] init];
  patterns_in_category = [[NSMutableArray alloc] init];
  //categories = [[NSArray alloc] initWithObjects:@"Category 1", @"Category 2",@"Category 3",@"Category 4",@"Category 5",nil];

  pattern_group_t* temp = g_pattern_lib->getCategories();
  NSString* name;
  
  [categoriesIndex addObject:@"0"];
  
  while (temp) {
    name = [NSString stringWithUTF8String: temp->name];
    
    [categories addObject:name];
    //NSString* str = [[NSString alloc] initWithFormat:@"%d", temp->index];
    //[categoriesIndex addObject:@"X"];
    [categoriesIndex addObject:[[NSString alloc] initWithFormat:@"%d", temp->index+1]];

    pattern_t* cur_pattern = g_pattern_lib->getCategory(temp->name); // first pattern in this category
    [patterns_in_category addObject:[NSValue valueWithPointer:cur_pattern]];
    
    temp = temp->next;
  }
  
  fakeItems = [[NSArray alloc] initWithObjects:@"Item 1",@"Item 2",@"Item 3",@"Item 4",@"Item 5",@"Item 6",@"Item 7",@"Item 8",@"Item 9",nil];
   
  //[table beginUpdates];
  //[table endUpdates];
   
  [super viewDidLoad];
}

- (void)viewDidDisappear:(BOOL)animated {
  [super viewDidDisappear:animated];
  //[table deselectRowAtIndexPath:[table indexPathForSelectedRow] animated:NO];
  [table reloadData];
}

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */


- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
  //[appDelegate showJuggler];
}

- (void)enterSiteViewControllerDidFinish:(EnterSiteViewController *)controller {
	[self dismissModalViewControllerAnimated:NO];
  [appDelegate showJuggler];
}


/*
- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
	
	controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
	[self presentModalViewController:controller animated:YES];
	
	[controller release];
}
 */

- (IBAction)done {
  [appDelegate showJuggler];
}

- (IBAction)settings {
  FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"SettingsView" bundle:nil];
  controller.delegate = self;
  
  controller.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
  [self presentModalViewController:controller animated:YES];
  
  [controller release];
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

- (void)setAppDelegate:(jmtouchAppDelegate*)appDelegate_ {
  appDelegate = appDelegate_;
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
  return [categories count] + 1;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  if (section == 0) 
    return [tableItems count];
  else {
    NSValue* ns_temp = [patterns_in_category objectAtIndex:section - 1];
    pattern_t* first_patt = (pattern_t*)[ns_temp pointerValue]; 

    return first_patt->pattern_count;
  }
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  static NSString *CellIdentifier = @"Cell";
  
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
  if (cell == nil) {
    cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
  }
  
  if (indexPath.section == 0) {
    if (indexPath.row <= 1)
      cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    else
      cell.accessoryType = UITableViewCellAccessoryNone;
  }
  else
    cell.accessoryType = UITableViewCellAccessoryNone;
  
  if (indexPath.section == 0) {
    NSString* str = [tableItems objectAtIndex:indexPath.row];
    
    if ([str isEqualToString:@"Set 2D Mode"] && g_jm->getRenderingMode() == RENDERING_OPENGL_2D)
        cell.textLabel.text = @"Set 3D Mode";
    else
        cell.textLabel.text = str;
  }
  else {
    const char* section_name = [[categories objectAtIndex:indexPath.section-1] cStringUsingEncoding:NSASCIIStringEncoding];
    
    NSValue* ns_temp = [patterns_in_category objectAtIndex:indexPath.section - 1];
    pattern_t* first_patt = (pattern_t*)[ns_temp pointerValue]; 
    
    // skip to correct pattern
    while (first_patt && first_patt->index < indexPath.row) first_patt = first_patt->next;
    
    NSString* str = [NSString stringWithUTF8String: first_patt->name];
    cell.textLabel.text = str;
  }
  
  return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
  if(section == 0)
    return nil;
  else
    return [categories objectAtIndex:section - 1];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  if (indexPath.section == 0) {
    NSString* str = [tableItems objectAtIndex:indexPath.row];
    
    if ([str isEqualToString:@"Enter Siteswap"]) {
      EnterSiteViewController *controller = [[EnterSiteViewController alloc] initWithNibName:@"EnterSiteView" bundle:nil];
      controller.delegate = self;
      
      controller.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
      [self presentModalViewController:controller animated:YES];
      
      [controller release];
      
    }
    else if ([str isEqualToString:@"Random Pattern"]) {
      g_pattern_lib->loadRandomPattern(g_jm, NULL, true);
      [appDelegate showJuggler];
    }
    else if ([str isEqualToString:@"Set 3D Mode"] || [str isEqualToString:@"Set 2D Mode"]) {
      jm->toggleRenderingMode();
      [appDelegate showJuggler];
    }
    else if ([str isEqualToString:@"Settings"]) {
      [self settings];
    }
    else if ([str isEqualToString:@"About"]) {
      
    }
  }
  else {
    NSValue* ns_temp = [patterns_in_category objectAtIndex:indexPath.section - 1];
    pattern_t* first_patt = (pattern_t*)[ns_temp pointerValue];
    g_pattern_lib->loadPattern(first_patt, indexPath.row, g_jm);
    [appDelegate showJuggler];
  }
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
  return categoriesIndex;
}

@end
