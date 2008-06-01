/* JMController */

//#import "JMView.h"
#include "jmlib.h"
#include "sqlite_patterns.h"
#import "JMOpenGLView.h"
#import <Cocoa/Cocoa.h>
#import <QuickTime/QuickTime.h>
#import <QTKit/QTKit.h>

#define TEXT_PROPORTION 35

#define MOVIE_KEY @"MovieKey"
#define FILENAME_KEY @"FilenameKey"
#define COMPONENT_KEY @"ComponentKey"
#define EXPORT_SETTINGS_KEY @"ExportSettingsKey"

typedef enum
{
	normalThrow = 0,
	singleThrow,
	karaoke
} pat;

@interface JMController : NSObject
{
	IBOutlet JMOpenGLView *view;
	IBOutlet NSImageView *errorReporter;
	IBOutlet NSMatrix *patternStyleButtons;
	IBOutlet NSPanel *inspectorPanel;
	IBOutlet NSMenuItem *recordingMenuItem;
	IBOutlet NSPopUpButton *styleSelect;
	IBOutlet NSPopUpButton *objectSelect;
	IBOutlet NSPopUpButton *categorySelect;
	IBOutlet NSPopUpButton *patternSelect;
  IBOutlet NSSegmentedControl *switchPattern;
	
	IBOutlet NSView *movieExportView;
	IBOutlet NSPopUpButton *movieExportSelection;
	IBOutlet NSProgressIndicator *movieExportProgress;
	
	@private
		JMLib *jm;
		NSTimer *renderTimer;
		BOOL showPattern;
		BOOL isRecording;
		BOOL encodeSheetShown;
		BOOL shouldCancelExport;
		QTMovie *movie;
		pat currentPat;
		DataHandler mDataHandlerRef;
    
    JMPatterns* patternLib;
    pattern_group_t* categories;
    pattern_t* cur_pattern; // patterns in currently selected category
}

- (IBAction)showInspector:(id)sender;
- (IBAction)setPattern:(id)sender;
- (IBAction)setStyle:(id)sender;
- (IBAction)setObjectType:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)toggleShowPattern:(id)sender;
- (IBAction)setPatternStyle:(id)sender;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)cancelExport:(id)sender;

- (IBAction)setLoadedCategory:(id)sender;
- (IBAction)setLoadedPattern:(id)sender;
- (IBAction)switchLoadedPattern:(id)sender;

- (void)updatePatterns:(NSString*)str;

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
