#include "appwindow.h"

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

#include <vector>

#include <ccg-ui/uicontext.h>

struct update_info
{
	claw::appwindow::updatefunc f;
	claw::appwindow::data *d;
	ccgui::mouse_input mouse;
};

@interface AppDelegate : NSObject
@end

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(NSNotification *)notification {

}

- (void)windowWillClose:(NSNotification *)notification {
	printf("Window closing, terminating app!\n");
	[[NSApplication sharedApplication] terminate: self];
}

@end

@interface TestView : NSOpenGLView
{
	CVDisplayLinkRef displayLink; //display link for managing rendering thread
	@public update_info uinfo;
	@public int viewWidth;
	@public int viewHeight;
}

-(void)drawRect:(NSRect)rect;
-(CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime;

@end

@implementation TestView

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	NSRect r = [self bounds];
	uinfo.mouse.x = mouseLoc.x;
	uinfo.mouse.y = r.size.height - mouseLoc.y - 1;
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	NSRect r = [self bounds];
	uinfo.mouse.x = mouseLoc.x;
	uinfo.mouse.y = r.size.height - mouseLoc.y - 1;
}

- (void)mouseDown:(NSEvent *)theEvent
{
	uinfo.mouse.primary.wentDown++;
	uinfo.mouse.primary.isDown = true;
	printf("Mouse down!\n");
}

- (void)mouseUp:(NSEvent *)theEvent
{
	uinfo.mouse.primary.wentUp++;
	uinfo.mouse.primary.isDown = false;
	printf("Mouse up!\n");
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

- (void)resetCursorRects
{
    [super resetCursorRects];
    printf("reset cursor rects\n");
    [self addCursorRect:self.bounds cursor:[NSCursor crosshairCursor]];
}

-(void)drawRect:(NSRect)rect {
	[[NSColor blueColor] set];
	NSRectFill( [self bounds] );
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	printf("afm\n");
	return YES;
}


- (void)prepareOpenGL
{
	// Synchronize buffer swaps with vertical refresh rate
	
	// (i.e. all openGL on this thread calls will go to this context)
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	
	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 32,
		0
	};
	
	CGLPixelFormatObj cglPixelFormat  = (CGLPixelFormatObj) [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, MyDisplayLinkCallback, self);
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	
	// Activate the display link
	CVDisplayLinkStart(displayLink);
	
	viewWidth = 100;
	viewHeight = 100;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
	TestView *p = (TestView*) displayLinkContext;
	
	NSSize    viewBounds = [p bounds].size;
	p->viewWidth = viewBounds.width;
	p->viewHeight = viewBounds.height;
	
	NSOpenGLContext *currentContext = [p openGLContext];
	[currentContext makeCurrentContext];

	// must lock GL context because display link is threaded
	CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
		
	CVReturn result = [(TestView*)displayLinkContext getFrameForTime:outputTime];
	
	// draw here
	[currentContext flushBuffer];
	CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
	
	return result;
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
	// Add your drawing codes here
	if (uinfo.f)
	{
		claw::appwindow::input_batch ib;
		ib.mouse = uinfo.mouse;
		uinfo.mouse.primary.wentUp = 0;
		uinfo.mouse.primary.wentDown = 0;
		uinfo.f(&ib, 0.005f);
		
	}
	else
		printf("Skipped frame\n");
	return kCVReturnSuccess;
}

- (void)dealloc
{
	// Release the display link
	CVDisplayLinkRelease(displayLink);
	
	[super dealloc];
}
@end


namespace claw
{
	namespace appwindow
	{
		struct data
		{
			NSAutoreleasePool *pool;
			NSApplication *app;
			NSWindow *window;
			TestView *view;
			AppDelegate *appdelegate;
		};
		
		data * create(const char *title, int width, int height)
		{
			data *d = new data();
			d->pool = [NSAutoreleasePool new];
			d->app = [NSApplication sharedApplication];
			d->appdelegate = [[AppDelegate alloc] autorelease];
			
		
			
			NSRect frame = NSMakeRect( 100., 100., 100. + (float)width, 100. + (float)height );
			
			d->window = [[NSWindow alloc]
					 initWithContentRect:frame
					 styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
					 backing:NSBackingStoreBuffered
					 defer:false];
			
			[d->window setTitle:@"Testing"];
			
			d->view = [[[TestView alloc] initWithFrame:frame] autorelease];
			d->view->uinfo.d = d;
			d->view->uinfo.mouse.x = 0;
			d->view->uinfo.mouse.y = 0;
			d->view->uinfo.mouse.primary.wentDown = 0;
			d->view->uinfo.mouse.primary.wentUp = 0;
			d->view->uinfo.mouse.primary.isDown = false;
			
			[d->window setContentView:d->view];
			[d->window makeKeyAndOrderFront:nil];
			[d->window setLevel:kCGMaximumWindowLevel];
			
			[d->app activateIgnoringOtherApps:YES];
			
			[NSApp setDelegate: (id)d->appdelegate];
			[d->window setDelegate: (id)d->appdelegate];
			
			[d->window setAcceptsMouseMovedEvents: true];
			[d->window makeFirstResponder: (id)d->view];
			
			int opts = (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
			NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[d->view bounds]
										 options:opts
										   owner:(id) d->view
										userInfo:nil];
			[d->view addTrackingArea:(id)area];
			
			return d;
			
		}
		
		void destroy(data *d)
		{
			[d->pool release];
		}
		
		void set_title(data *d, const char *title)
		{
			
		}
		
		void get_client_rect(data *d, int *x0, int *y0, int *x1, int *y1)
		{
			*x0 = 0;
			*y0 = 0;
			*x1 = d->view->viewWidth;
			*y1 = d->view->viewHeight;
			return;
		}
		
		void run_loop(data *d, updatefunc f)
		{
			d->view->uinfo.f = f;
			[d->app run];
		}
		
	}
}

