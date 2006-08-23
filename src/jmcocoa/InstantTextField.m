#import "InstantTextField.h"

@implementation InstantTextField

- (void)keyUp:(NSEvent *)event
{
	[self sendAction:[self action] to:[self target]];
}

@end
