#import  "CCView.h"

@implementation CCView
- (void)onCreate
{
  printf( "CCView created\n" );
}

- (void)onDraw
{
  // Override as desired
  renderer->set_clear_color( 0xff0000ff );
  renderer->clear( Renderer::CLEAR_COLOR );
  
  //NSLog( @"This view's name is '%@'\n", self.name );
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

@end


