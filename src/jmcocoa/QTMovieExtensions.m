#import "QTMovieExtensions.h"
#include <stdlib.h>

@interface QTMovie(PrivateExtensions)

+(Movie)quicktimeMovieFromTempFile:(DataHandler *)outDataHandler error:(OSErr *)outErr;

@end

@implementation QTMovie (QTMovieExtensions)

- (void)addImageToMovie:(NSImage *)image
{
	NSDictionary *settingsDict = nil;

	settingsDict = [NSDictionary dictionaryWithObjects:
		[NSArray arrayWithObjects:
			@"tiff",
			[NSNumber numberWithLong:codecHighQuality],
			nil]
											   forKeys:
		[NSArray arrayWithObjects:
			QTAddImageCodecType,
			QTAddImageCodecQuality,
			nil]];
	if (!settingsDict)
	{
		return;
	}

	[self addImage:image forDuration:QTMakeTime(30, 600) withAttributes:settingsDict];
}

+(QTMovie *)movieForAddingImagesWithFirstFrame:(NSImage *)firstFrame
{
	NSError *err;
	QTMovie *qtMovie = [QTMovie movieWithFile:[[NSBundle mainBundle] pathForResource:@"emptyMovie" ofType:@"mov"] error:&err];
	
	[qtMovie setAttribute:[NSNumber numberWithBool:YES] forKey:QTMovieEditableAttribute];
	[qtMovie addImageToMovie:firstFrame];
	[qtMovie deleteSegment:QTMakeTimeRange(QTMakeTime(0,0),QTMakeTime(60,600))];
	
	return qtMovie;
}


+(Movie)quicktimeMovieFromTempFile:(DataHandler *)outDataHandler error:(OSErr *)outErr
{
	Handle	dataRefH = nil;
	OSType	dataRefType;
	Movie	qtMovie	= nil;

	*outErr = -1;
	
	// generate a name for our movie file
	NSString *tempName = [NSString stringWithCString:tmpnam(nil) 
											encoding:[NSString defaultCStringEncoding]];
	if (!tempName)
	{
		return nil;
	}
	
	// create a file data reference for our movie
	*outErr = QTNewDataReferenceFromFullPathCFString((CFStringRef)tempName,
													 kQTNativeDefaultPathStyle,
													 0,
													 &dataRefH,
													 &dataRefType);
	if (*outErr != noErr)
	{
		return nil;
	}
	
	// create a QuickTime movie from our file data reference
	CreateMovieStorage (dataRefH,
						dataRefType,
						'TVOD',
						smSystemScript,
						newMovieActive, 
						outDataHandler,
						&qtMovie);
	*outErr = GetMoviesError();
 	if (*outErr != noErr) 
	{
		DisposeHandle(dataRefH);
		return nil;
	}
	
	return qtMovie;
}


//
// buildQTKitMovie
//
// Build a QTKit movie from a series of image frames
//
//

+(QTMovie *)qtMovieWithDataHandler:(DataHandler *)dh
{
	OSErr err;
	// create a QuickTime movie
	Movie qtMovie = [QTMovie quicktimeMovieFromTempFile:dh error:&err];
	QTMovie *mMovie;
	if (!qtMovie || err)
	{
		return nil;
	}
	
	// instantiate a QTMovie from our QuickTime movie
	mMovie = [QTMovie movieWithQuickTimeMovie:qtMovie disposeWhenDone:YES error:nil];
	if (!mMovie || err) 
	{
		return nil;
	}
	
	// mark the movie as editable
	[mMovie setAttribute:[NSNumber numberWithBool:YES] forKey:QTMovieEditableAttribute];
	
	return mMovie;
}

@end
