#import  "StarbrightNSOpenGLView.h"
using namespace SuperCPP;
using namespace Starbright;

#define SB_BRIDGE_RETAIN( objc_ptr ) ((void*)CFBridgingRetain(objc_ptr))
#define SB_BRIDGE_CAST( ptr, type )  ((__bridge type) ptr)

static CVReturn StarbrightNSView_display_link_callback( CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time,
    CVOptionFlags input_flags, CVOptionFlags* output_flags, void* view_ptr )
{
  static bool locked = false;
  if (locked) return kCVReturnSuccess;
  locked = true;

  StarbrightNSOpenGLView* view = SB_BRIDGE_CAST( view_ptr, StarbrightNSOpenGLView* );

  if (view.window.isVisible)
  {
    [view drawRect:view.bounds];
  }
  else
  {
    [view stopDisplayLink];
    [view onStop];
  }

  locked = false;
  return kCVReturnSuccess;
}



@implementation StarbrightNSOpenGLView
- (void)startDisplayLink
{
  if (display_link) return;

  NSLog( @"StarbrightNSOpenGLView starting display link" );
  CGLLockContext( [[self openGLContext] CGLContextObj] );

  GLint swap_interval = 1;
  [[self openGLContext] setValues:&swap_interval forParameter:NSOpenGLCPSwapInterval];

  CVDisplayLinkCreateWithActiveCGDisplays( &display_link );
  CVDisplayLinkSetOutputCallback( display_link, StarbrightNSView_display_link_callback, SB_BRIDGE_RETAIN(self) );

  // Set the display link for the current renderer
  CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
  CGLPixelFormatObj cglPixelFormat = [[NSOpenGLView defaultPixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext( display_link, cglContext, cglPixelFormat );

  // Activate the display link
  CVDisplayLinkStart( display_link );
  CGLUnlockContext( [[self openGLContext] CGLContextObj] );
}

- (void)stopDisplayLink
{
  if (display_link)
  {
    NSLog( @"StarbrightNSOpenGLView stopping display link" );
    CVDisplayLinkStop( display_link );
    display_link = nil;
  }
}

- (void)drawRect:(NSRect)bounds
{
  if ( !initialized_starbright )
  {
    renderer = new OpenGLRenderer();
    initialized_starbright = YES;
    [self onCreate];
  }

  if ( !display_link )
  {
    [self startDisplayLink];
    [self onStart];
  }

  CGLLockContext( [[self openGLContext] CGLContextObj] );
  [[self openGLContext] makeCurrentContext];

  double current_timestamp = SuperCPP::current_time_seconds();
  double elapsed_time = current_timestamp - previous_frame_timestamp;
  previous_frame_timestamp = current_timestamp;

  if (elapsed_time < 0 || elapsed_time > 1.0/10.0)
  {
    // If there was more than a 1/10th second delay, assume some multitask or
    // I/O hangup caused the delay and don't worry about trying to make the
    // time up - just update a single frame's worth and draw.
    time_debt = 1.0 / 60.0;
  }
  else
  {
    time_debt += elapsed_time;
  }

  while (time_debt >= 1.0/60.0)
  {
    time_debt -= 1.0/60.0;
    [self onUpdate];
  }

  [self onDraw];

  glFlush();

  CGLUnlockContext( [[self openGLContext] CGLContextObj] );
}

- (void)onCreate
{
}

- (void)onDraw
{
  // Override as desired
}

- (void)onStart
{
}

- (void)onStop
{
}

- (void)onUpdate
{
  // Override as desired
}

- (void)reshape
{
  NSRect rect = [self bounds];
  display_width  = NSWidth(rect);
  display_height = NSHeight(rect);
}

- (void)viewDidHide
{
  printf( "View hidden\n" );
}

- (void)viewDidUnhide
{
  printf( "View shown\n" );
}

@end

