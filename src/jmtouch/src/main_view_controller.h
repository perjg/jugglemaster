//
//  MainViewController.h
//  jmtouch2
//
//  Created by 葛亮 on 09/07/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "settings_view_controller.h"
#include "jmlib.h"

@class jmtouchAppDelegate;
 
//@interface MainViewController : UINavigationController <FlipsideViewControllerDelegate> {
@interface MainViewController : UIViewController <FlipsideViewControllerDelegate> {
  IBOutlet UITableView *table;
  
  @private
    NSArray* tableItems;
  
    NSArray* categories;
    NSArray* fakeItems;
  
    jmtouchAppDelegate* appDelegate;
    JMLib* jm;
}

@property JMLib* jm;
- (IBAction)done;
- (void)setAppDelegate:(jmtouchAppDelegate*)appDelegate_;
//- (void)setJMLib:(JMLib*)jm_;

@end
