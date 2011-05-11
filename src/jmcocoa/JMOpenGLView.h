/* JMOpenGLView */

#import <Cocoa/Cocoa.h>
#include "jmlib.h"

@interface JMOpenGLView : NSOpenGLView
{
    IBOutlet id dataSource;
    JMLib* jm;
}

- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;

- (IBAction)copy:(id)sender;
- (void)setJMLib:(JMLib*)jm_;

@end
