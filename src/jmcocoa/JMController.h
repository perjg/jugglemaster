/* JMConterller */

#import <Cocoa/Cocoa.h>

@interface JMController : NSObject
{
	JMLib *jm;
	IBOutlet JMView *view;
	IBOutlet NSImageView *errorReporter;
	NSTimer *renderTimer;
	BOOL showPattern;
}

- (IBAction)showInspector:(id)sender;
- (IBAction)setPattern:(id)sender;
- (IBAction)setStyle:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)toggleShowPattern:(id)sender;

- (void)setFrame:(NSRect)frameRect;

- (arm)bodyPartPositions;
- (int)ballDiameter;
- (int)numberOfBalls;
- (NSPoint)ballPosition:(int)ballNum;
- (NSColor *)ballColour:(int)ballNum;

- (BOOL)shouldShowPattern;
- (NSString *)currentThrow;

@end
