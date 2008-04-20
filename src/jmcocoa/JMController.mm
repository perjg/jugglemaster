#import "QTMovieExtensions.h"
#import "JMView.h"
#include "jmlib.h"
#include "validator.h"
#import "JMController.h"
#include <math.h>

@interface JMController(Private)

- (void)doRender:(NSTimer *)timer;
- (void)doUpdate:(NSTimer *)timer;

-(QTMovie *)buildQTKitMovieFromImages:(NSArray *)images;

- (NSArray *)availableComponents;
- (NSData *)getExportSettingsForComponent:(NSDictionary *)comp;
- (BOOL)writeMovie:(QTMovie *)movie toFile:(NSString *)file withComponent:(NSDictionary *)component withExportSettings:(NSData *)exportSettings;

@end

@implementation JMController

static JML_CHAR patterns[7][12]
	= {"Normal", "Reverse", "Shower", "Mills Mess",
	   "Center", "Windmill", "Random"};
	
- (id)init
{
	if (self = [super init])
	{
    jm = JMLib::alloc_JuggleMaster();
		
		jm->setWindowSize([view frame].size.width, [view frame].size.height);
		jm->setPatternDefault();
		jm->setStyleDefault();
		jm->setSpeed(1.0);
        jm->setScalingMethod(SCALING_METHOD_DYNAMIC);
		jm->startJuggle();
		jm->doJuggle();
		
		showPattern = NO;
		currentPat = karaoke;
		encodeSheetShown = NO;
		
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

- (IBAction)copy:(id)sender
{
	[view copy:sender];
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

- (IBAction)toggleRecording:(id)sender
{
	NSArray *exportComponents;
	//MovieExportComponent exporter;
	NSEnumerator *componentEnumerator;
	NSDictionary *currentComponentDescription;
	NSSavePanel *savePanel;
	
	if (isRecording = !isRecording)
	{
		[recordingMenuItem setTitle:@"Stop Movie Capture"];
		//movie = [[QTMovie qtMovieWithDataHandler:&mDataHandlerRef] retain];
		movie = [[QTMovie movieForAddingImagesWithFirstFrame:[view frameGrab]] retain];
	}
	else
	{
		exportComponents = [self availableComponents];
		componentEnumerator = [exportComponents objectEnumerator];
		[movieExportSelection removeAllItems];
		while (currentComponentDescription = [componentEnumerator nextObject])
		{
			[movieExportSelection addItemWithTitle:[currentComponentDescription objectForKey:@"name"]];
		}
		shouldCancelExport = NO;
		savePanel = [NSSavePanel savePanel];
		[savePanel setAccessoryView:movieExportView];
		
		[savePanel beginSheetForDirectory:@"~/"
									 file:nil
						   modalForWindow:[view window]
							modalDelegate:self
						   didEndSelector:@selector(movieExportSheetDidEnd:returnCode:contextInfo:)
							  contextInfo:nil];
				
		[recordingMenuItem setTitle:@"Start Movie Capture"];
	}
}

- (IBAction)cancelExport:(id)sender
{
	shouldCancelExport = YES;
}

- (void)movieExportSheetDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void  *)contextInfo
{
	NSData *exportSettings;
	NSDictionary *componentToUse;
	
	[sheet close];
	
	if (returnCode == NSOKButton)
	{
		componentToUse = [[self availableComponents] objectAtIndex:[movieExportSelection indexOfSelectedItem]];
		exportSettings = [self getExportSettingsForComponent:componentToUse];
		[movie setDelegate:self];
		[NSThread detachNewThreadSelector:@selector(doWrite:)
								 toTarget:self
							   withObject:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					movie,
					[sheet filename],
					componentToUse,
					exportSettings,
					nil]
										forKeys:
				[NSArray arrayWithObjects:
					MOVIE_KEY,
					FILENAME_KEY,
					COMPONENT_KEY,
					EXPORT_SETTINGS_KEY,
					nil]]];
	}
}

- (void)doWrite:(NSDictionary *)writeSettings
{
	NSAutoreleasePool *autoreleasePool = [[NSAutoreleasePool alloc] init];
	[self writeMovie:[writeSettings objectForKey:MOVIE_KEY]
			  toFile:[writeSettings objectForKey:FILENAME_KEY]
	   withComponent:[writeSettings objectForKey:COMPONENT_KEY]
  withExportSettings:[writeSettings objectForKey:EXPORT_SETTINGS_KEY]];
	[autoreleasePool release];
}

- (NSArray *)availableComponents
{
	NSMutableArray *array = [NSMutableArray array];
	
	ComponentDescription cd;
	Component c;
	
	cd.componentType = MovieExportType;
	cd.componentSubType = 0;
	cd.componentManufacturer = 0;
	cd.componentFlags = canMovieExportFiles + movieExportMustGetSourceMediaType;
	cd.componentFlagsMask = canMovieExportFiles + movieExportMustGetSourceMediaType;
	
	while((c = FindNextComponent(c, &cd)))
	{
		Handle name = NewHandle(4);
		ComponentDescription exportCD;
		
		if (GetComponentInfo(c, &exportCD, name, nil, nil) == noErr)
		{
			unsigned char *namePStr = (unsigned char *)*name;
			NSString *nameStr = [[NSString alloc] initWithBytes:&namePStr[1] length:namePStr[0] encoding:NSUTF8StringEncoding];
			
			NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:
				nameStr, @"name",
				[NSData dataWithBytes:&c length:sizeof(c)], @"component",
				[NSNumber numberWithLong:exportCD.componentType], @"type",
				[NSNumber numberWithLong:exportCD.componentSubType], @"subtype",
				[NSNumber numberWithLong:exportCD.componentManufacturer], @"manufacturer",
				nil];
			[array addObject:dictionary];
			[nameStr release];
		}
		
		DisposeHandle(name);
	}
	return array;
}

- (NSData *)getExportSettingsForComponent:(NSDictionary *)comp
{
	Component c;
	memcpy(&c, [[comp objectForKey:@"component"] bytes], sizeof(c));
	
	MovieExportComponent exporter = OpenComponent(c);
	Boolean canceled;
	ComponentResult err = MovieExportDoUserDialog(exporter, NULL, NULL, 0, 0, &canceled);
	if(err)
	{
		NSLog(@"Got error %d when calling MovieExportDoUserDialog");
		CloseComponent(exporter);
		return nil;
	}
	if(canceled)
	{
		CloseComponent(exporter);
		return nil;
	}
	QTAtomContainer settings;
	err = MovieExportGetSettingsAsAtomContainer(exporter, &settings);
	if(err)
	{
		NSLog(@"Got error %d when calling MovieExportGetSettingsAsAtomContainer");
		CloseComponent(exporter);
		return nil;
	}
	NSData *data = [NSData dataWithBytes:*settings length:GetHandleSize(settings)];
	DisposeHandle(settings);
	
	CloseComponent(exporter);
	
	return data;
}

- (BOOL)writeMovie:(QTMovie *)qtMovie toFile:(NSString *)file withComponent:(NSDictionary *)component withExportSettings:(NSData *)exportSettings
{
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:YES], QTMovieExport,
		[component objectForKey:@"subtype"], QTMovieExportType,
		[component objectForKey:@"manufacturer"], QTMovieExportManufacturer,
		exportSettings, QTMovieExportSettings,
		nil];
	
	BOOL result = [qtMovie writeToFile:file withAttributes:attributes];
	if(!result)
	{
		NSLog(@"Couldn't write movie to file");
		return NO;
	}
	
	return YES;
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
	return jm->numBalls();
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
	if (isRecording)
	{
		[movie addImageToMovie:[view frameGrab]];
	}
}

- (BOOL)shouldShowPattern
{
	return showPattern;
}

- (NSAttributedString *)currentThrowString
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

		return [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"Pattern: %@", [NSString stringWithCString:(char *)site]]
												attributes:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					[NSFont fontWithName:@"Futura" size:[view frame].size.width / TEXT_PROPORTION],
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
		currentSwap = [[[NSMutableAttributedString alloc] initWithString:[NSString stringWithFormat:@"Pattern: %@", [NSString stringWithCString:(char *)site]]
															  attributes:
			[NSDictionary dictionaryWithObjects:
				[NSArray arrayWithObjects:
					[NSFont fontWithName:@"Futura" size:[view frame].size.width / TEXT_PROPORTION],
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
			[currentSwap addAttribute:NSForegroundColorAttributeName value:[NSColor yellowColor] range:NSMakeRange(start + 9, stop - start)];
		}
	}
	
	return currentSwap;
}

- (NSAttributedString *)juggleStyleString
{
	NSMutableParagraphStyle *paragraphStyle;
	
	paragraphStyle = [[[NSMutableParagraphStyle alloc] init] autorelease];
	[paragraphStyle setAlignment:NSRightTextAlignment];
	
	return [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"Style: %@", [[styleSelect selectedItem] title]]
											attributes:
		[NSDictionary dictionaryWithObjects:
			[NSArray arrayWithObjects:
				[NSFont fontWithName:@"Futura" size:[view frame].size.width / TEXT_PROPORTION],
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

- (NSAttributedString *)numberOfBallsString
{
	return [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"Balls: %d", [self numberOfBalls]]
											attributes:
		[NSDictionary dictionaryWithObjects:
			[NSArray arrayWithObjects:
				[NSFont fontWithName:@"Futura" size:[view frame].size.width / TEXT_PROPORTION],
				[NSColor lightGrayColor],
				nil]
									forKeys:
			[NSArray arrayWithObjects:
				NSFontAttributeName,
				NSForegroundColorAttributeName,
				nil]]] autorelease];
}

- (pat)patternStyle
{
	return currentPat;
}

- (BOOL)movie:(QTMovie *)movie shouldContinueOperation:(NSString *)op withPhase:(QTMovieOperationPhase)phase atPercent:(NSNumber *)percent withAttributes:(NSDictionary *)attributes
{
	NSRect viewFrame = [view frame];
	
	if (phase == QTMovieOperationBeginPhase)
	{
		viewFrame.size.height -= 20;
		viewFrame.origin.y += 20;
		encodeSheetShown = YES;
		[movieExportProgress setHidden:NO];
		[view setFrame:viewFrame];
		[[view window] display];
	}
	else if (phase == QTMovieOperationEndPhase)
	{
		viewFrame.size.height += 20;
		viewFrame.origin.y -= 20;
		encodeSheetShown = NO;
		[movieExportProgress setHidden:YES];
		[view setFrame:viewFrame];
		[[view window] display];
	}
		
	[movieExportProgress setDoubleValue:[percent doubleValue]];
	[movieExportProgress display];
	
	if (shouldCancelExport)
	{
		return NO;
	}
	else
	{
		return YES;
	}
}

@end
