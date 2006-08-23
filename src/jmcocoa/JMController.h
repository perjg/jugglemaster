/* JMConterller */

#import <Cocoa/Cocoa.h>

typedef struct shead
{
	float x;
	float y;
	float r;
} head;

@interface JMController : NSObject
{
	JMLib *jm;
	IBOutlet JMView *view;
	IBOutlet NSImageView *errorReporter;
	NSTimer *renderTimer;
}

- (IBAction)showInspector:(id)sender;
- (IBAction)setPattern:(id)sender;
- (IBAction)setStyle:(id)sender;
- (IBAction)setSpeed:(id)sender;

- (void)setFrame:(NSRect)frameRect;

- (arm)bodyPartPositions;
- (int)ballDiameter;
- (int)numberOfBalls;
- (NSPoint)ballPosition:(int)ballNum;
- (NSColor *)ballColour:(int)ballNum;

@end
