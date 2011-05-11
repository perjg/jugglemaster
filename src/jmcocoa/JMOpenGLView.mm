#import "JMOpenGLView.h"
#import "JMController.h"
#include <OpenGL/gl.h>

@interface JMOpenGLView(Private)

@end

@implementation JMOpenGLView

- (id) initWithFrame: (NSRect) frame
{
	GLuint attribs[] = 
	{
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAWindow,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAAccumSize, 0,
		0
	};

	NSOpenGLPixelFormat* fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: (NSOpenGLPixelFormatAttribute*) attribs]; 
	
	if (!fmt)
		NSLog(@"No OpenGL pixel format");

	return self = [super initWithFrame:frame pixelFormat: [fmt autorelease]];
}

- (void) awakeFromNib
{
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void) drawRect: (NSRect) rect
{
  if (jm) jm->render();	
	[[self openGLContext] flushBuffer];
}

- (void)reshape
{
	if (jm) jm->setWindowSize([self frame].size.width, [self frame].size.height);
}

- (void)setJMLib:(JMLib*)jm_
{
	jm = jm_;
}

- (IBAction)copy:(id)sender
{
	//[[NSPasteboard generalPasteboard] declareTypes:[NSArray arrayWithObject:NSTIFFPboardType] owner:self];
	//[[NSPasteboard generalPasteboard] setData:[[self frameGrab] TIFFRepresentation] forType:NSTIFFPboardType];
}

//NSPoint p;

- (void)mouseDown:(NSEvent *)event
{
  if ([event clickCount] > 1) {
    jm->resetCamera();
  }
  else {
    NSPoint p = [event locationInWindow];
    jm->trackballStart(p.x, [self frame].size.height - p.y);
  }
}

- (void)mouseDragged:(NSEvent *)event
{
  NSPoint p = [event locationInWindow];
  jm->trackballTrack(p.x, [self frame].size.height - p.y);
}

NSPoint cur;

- (void)rightMouseDown:(NSEvent *)event
{
  NSPoint p = [event locationInWindow];
  cur.x = p.x;
  cur.y = p.y;
}

- (void)rightMouseDragged:(NSEvent *)event
{
  NSPoint p = [event locationInWindow];

  float dx = -(p.x - cur.x)*10.0f / [self frame].size.width;
  float dy = -(p.y - cur.y)*10.0f  / [self frame].size.height;

  jm->setAutoRotate(false);
  jm->move(dx, dy);
  cur.x = p.x;
  cur.y = p.y;
}

@end
