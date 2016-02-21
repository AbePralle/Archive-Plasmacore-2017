#import  "PlasmacoreView.h"

@implementation PlasmacoreView
- (void)onCreate
{
  NSLog( @"PlasmacoreView '%@' created\n", [self valueForKey:@"name"] );
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


