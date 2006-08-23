#import "JMView.h"
#include "jmlib.h"
#include "validator.h"
#import "JMController.h"
#include "math.h"

@interface JMController(Private)

- (void)doRender:(NSTimer *)timer;
- (void)doUpdate:(NSTimer *)timer;

@end

@implementation JMController

static JML_CHAR patterns[7][12]
	= {"Normal", "Reverse", "Shower", "Mills Mess",
	   "Center", "Windmill", "Random"};
	
- (id)init
{
	if (self = [super init])
	{
		jm = new JMLib();
		
		jm->setWindowSize([view frame].size.width, [view frame].size.height);
		jm->setPatternDefault();
		jm->setStyleDefault();
		jm->setSpeed(1.0);
		jm->startJuggle();
		jm->doJuggle();
		
		showPattern = NO;
		
		[self showInspector:self];
		
		renderTimer = [[NSTimer scheduledTimerWithTimeInterval:1/30 target:self selector:@selector(doRender:) userInfo:nil repeats:YES] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[renderTimer invalidate];
	
	[renderTimer release];
	[super dealloc];
}

- (IBAction)showInspector:(id)sender
{
	[NSBundle loadNibNamed:@"JMInspector" owner:self];
}

- (IBAction)setPattern:(id)sender
{
	NSString *pattern = [sender stringValue];
	JML_CHAR *pat = (JML_CHAR *)[pattern cString];
	
	if (JMSiteValidator::validateSite(pat))
	{
		[errorReporter setHidden:YES];
		jm->stopJuggle();
		jm->setPattern("", pat);
		jm->startJuggle();
	}
	else
	{
		[errorReporter setHidden:NO];
		jm->stopJuggle();
	}
}

- (IBAction)setStyle:(id)sender
{
	jm->setStyle(patterns[[[sender selectedItem] tag]]);
}

- (IBAction)setSpeed:(id)sender
{
	jm->stopJuggle();
	jm->setSpeed([sender floatValue]);
	jm->startJuggle();
}

- (IBAction)toggleShowPattern:(id)sender
{
	showPattern = !showPattern;
}

- (void)setFrame:(NSRect)frameRect
{
	jm->setWindowSize(frameRect.size.width, frameRect.size.height);
	jm-> doJuggle();
	[view setNeedsDisplay:YES];
}

- (arm)bodyPartPositions
{	
	return jm->ap;
}

- (int)ballDiameter
{
	return (11*jm->dpm/DW);	
}

- (int)numberOfBalls
{
	return jm->balln;
}

- (NSPoint)ballPosition:(int)ballNum
{
	return NSMakePoint(jm->b[ballNum].gx, jm->b[ballNum].gy);
}

- (NSColor *)ballColour:(int)ballNum
{
	return [NSColor redColor];
}

- (void)doRender:(NSTimer *)timer
{
	jm->doJuggle();
	[view setNeedsDisplay:YES];
}

- (BOOL)shouldShowPattern
{
	return showPattern;
}

- (NSString *)currentThrow
{
	int start = jm->getSiteposStart();
	int stop = jm->getSiteposStop();
	int current;
	JML_CHAR *site = jm->getSite();
	JML_CHAR *subString = (JML_CHAR *)malloc((stop - start + 1) * sizeof(JML_CHAR));
	
	for (current = start; current < stop; current++)
	{
		subString[current - start] = site[current];
	}
	subString[current - start] = '\0';
	
	return [NSString stringWithCString:(char *)subString];
}

@end
