#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#import <QuartzCore/CAMetalLayer.h>

namespace Nz
{
	id CreateAndAttachMetalLayer(void* window) {
		NSWindow* obj = (__bridge NSWindow*)window;
		NSView* view = [[NSView alloc] initWithFrame:obj.frame];
		[view setLayer:[CAMetalLayer layer]];
		[view setWantsLayer:YES];
		view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
		[obj.contentView addSubview:view];
		return view.layer;
	}
}
