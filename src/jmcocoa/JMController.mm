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
		currentPat = karaoke;
		
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
	if (!inspectorPanel)
	{
		[NSBundle loadNibNamed:@"JMInspector" owner:self];
		[patternStyleButtons setEnabled:NO];
	}
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
	[patternStyleButtons setEnabled:![patternStyleButtons isEnabled]];
}

- (IBAction)setPatternStyle:(id)sender
{
	currentPat = (pat)[sender selectedRow];
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
	return jm->getBallRadius()*2;	
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

- (NSAttributedString *)currentThrow
{
	int start = jm->getSiteposStart();
	int stop = jm->getSiteposStop();
	int current;
	JML_CHAR *site = jm->getSite();
	JML_CHAR *subString = (JML_CHAR *)malloc((stop - start + 1) * sizeof(JML_CHAR));
	
	NSMutableAttributedString *currentSwap;
	NSMutableParagraphStyle *paragraphStyle;
	
	paragraphStyle = [[[NSMutableParagraphStyle alloc] init] autorelease];
	[paragraphStyle setAlignment:NSCenterTextAlignment];
		
	if ([self patternStyle] == singleThrow)
	{
		for (current = start; current < stop; current++)
		{
			subString[current - start] = site[current];
		}
		subString[current - start] = '\0';

		return [[[NSAttributedString alloc] initWithString:[NSString stringWithCString:(char *)subString]
												attributes:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					[NSFont fontWithName:@"Futura" size:32],
					[NSColor lightGrayColor],
					paragraphStyle,
					nil]
										forKeys:
				[NSArray arrayWithObjects:
					NSFontAttributeName,
					NSForegroundColorAttributeName,
					NSParagraphStyleAttributeName,
					nil]]] autorelease];
	}
	else
	{
		currentSwap = [[[NSMutableAttributedString alloc] initWithString:[NSString stringWithCString:(char *)site]
															  attributes:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					[NSFont fontWithName:@"Futura" size:32],
					[NSColor lightGrayColor],
					paragraphStyle,
					nil]
										forKeys:
				[NSArray arrayWithObjects:
					NSFontAttributeName,
					NSForegroundColorAttributeName,
					NSParagraphStyleAttributeName,
					nil]]] autorelease];
		if ([self patternStyle] == karaoke)
		{
			[currentSwap addAttribute:NSForegroundColorAttributeName value:[NSColor yellowColor] range:NSMakeRange(start, stop - start)];
		}
	}
	
	return currentSwap;
}

- (NSAttributedString *)juggleStyle
{
	NSMutableParagraphStyle *paragraphStyle;
	
	paragraphStyle = [[[NSMutableParagraphStyle alloc] init] autorelease];
	[paragraphStyle setAlignment:NSRightTextAlignment];
	
	return [[[NSAttributedString alloc] initWithString:[[styleSelect selectedItem] title]
											attributes:
		[NSDictionary dictionaryWithObjects:
			[NSArray arrayWithObjects:
				[NSFont fontWithName:@"Futura" size:32],
				[NSColor lightGrayColor],
				paragraphStyle,
				nil]
								   forKeys:
			[NSArray arrayWithObjects:
				NSFontAttributeName,
				NSForegroundColorAttributeName,
				NSParagraphStyleAttributeName,
				nil]]] autorelease];
}

- (pat)patternStyle
{
	return currentPat;
}

@end
