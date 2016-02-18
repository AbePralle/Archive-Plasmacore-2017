#import  "PlasmacoreView.h"

@implementation PlasmacoreView
- (void)onCreate
{
  printf( "PlasmacoreView created\n" );
}

- (void)onDraw
{
  // Override as desired
  renderer->set_clear_color( 0xff0000ff );
  renderer->clear( Renderer::CLEAR_COLOR );
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


