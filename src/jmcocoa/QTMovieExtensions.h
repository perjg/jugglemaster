#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>

@interface QTMovie (QTMovieExtensions)

+ (QTMovie *)movieForAddingImagesWithFirstFrame:(NSImage *)firstFrame;
+ (QTMovie *)qtMovieWithDataHandler:(DataHandler *)dh;

- (void)addImageToMovie:(NSImage *)imageFilesArray;

@end