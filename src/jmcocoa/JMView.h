/* JMView */

#import <Cocoa/Cocoa.h>

@interface JMView : NSView
{
    IBOutlet id dataSource;
}

- (IBAction)copy:(id)sender;

- (NSImage *)frameGrab;

@end
