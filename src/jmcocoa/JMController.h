/* JMConterller */

#import <Cocoa/Cocoa.h>

typedef enum
{
	normalThrow = 0,
	singleThrow,
	karaoke
} pat;

@interface JMController : NSObject
{
	JMLib *jm;
	IBOutlet JMView *view;
	IBOutlet NSImageView *errorReporter;
	IBOutlet NSMatrix *patternStyleButtons;
	IBOutlet NSPanel *inspectorPanel;
	IBOutlet NSPopUpButton *styleSelect;
	NSTimer *renderTimer;
	BOOL showPattern;
	pat currentPat;
}

- (IBAction)showInspector:(id)sender;
- (IBAction)setPattern:(id)sender;
- (IBAction)setStyle:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)toggleShowPattern:(id)sender;
- (IBAction)setPatternStyle:(id)sender;

- (void)setFrame:(NSRect)frameRect;

- (arm)bodyPartPositions;
- (int)ballDiameter;
- (int)numberOfBalls;
- (NSPoint)ballPosition:(int)ballNum;
- (NSColor *)ballColour:(int)ballNum;

- (BOOL)shouldShowPattern;
- (pat)patternStyle;
- (NSAttributedString *)currentThrow;
- (NSAttributedString *)juggleStyle;

@end
