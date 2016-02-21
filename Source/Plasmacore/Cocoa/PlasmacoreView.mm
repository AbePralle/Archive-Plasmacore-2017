#import  "Plasmacore.h"
#import  "PlasmacoreView.h"

@implementation PlasmacoreView
- (void)onCreate
{
  NSLog( @"PlasmacoreView '%@' created\n", [self valueForKey:@"name"] );
}

- (void)onDraw
{
  // Override as desired
  PlasmacoreMessage* m_draw = [PlasmacoreMessage messageWithType:"Window.draw"];
  [m_draw setInt32:"window_id" value:[[Plasmacore singleton] getWindowID:self.window.windowController]];
  [m_draw setString:"view_name" value:[self valueForKey:@"name"] ];
  [m_draw push];

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


