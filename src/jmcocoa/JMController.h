/* JMConterller */

#import <Cocoa/Cocoa.h>
#import <QuickTime/QuickTime.h>
#import <QTKit/QTKit.h>

#define TEXT_PROPORTION 35

typedef enum
{
	normalThrow = 0,
	singleThrow,
	karaoke
} pat;

@interface JMController : NSObject
{
	IBOutlet JMView *view;
	IBOutlet NSImageView *errorReporter;
	IBOutlet NSMatrix *patternStyleButtons;
	IBOutlet NSPanel *inspectorPanel;
	IBOutlet NSPopUpButton *styleSelect;
	IBOutlet NSMenuItem *recordingMenuItem;
	
	IBOutlet NSPanel *movieExportPanel;
	IBOutlet NSPopUpButton *movieExportSelection;
	IBOutlet NSButton *exportButton;
	
	@private
		JMLib *jm;
		NSTimer *renderTimer;
		BOOL showPattern;
		BOOL isRecording;
		QTMovie *movie;
		pat currentPat;
		DataHandler mDataHandlerRef;
}

- (IBAction)showInspector:(id)sender;
- (IBAction)setPattern:(id)sender;
- (IBAction)setStyle:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)toggleShowPattern:(id)sender;
- (IBAction)setPatternStyle:(id)sender;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)endExportSheet:(id)sender;

- (void)setFrame:(NSRect)frameRect;

- (arm)bodyPartPositions;
- (int)ballDiameter;
- (int)numberOfBalls;
- (NSPoint)ballPosition:(int)ballNum;
- (NSColor *)ballColour:(int)ballNum;

- (BOOL)shouldShowPattern;
- (pat)patternStyle;
- (NSAttributedString *)currentThrowString;
- (NSAttributedString *)juggleStyleString;
- (NSAttributedString *)numberOfBallsString;

@end
