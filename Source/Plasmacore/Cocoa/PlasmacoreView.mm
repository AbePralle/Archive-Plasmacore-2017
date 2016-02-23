#import  "Plasmacore.h"
#import  "PlasmacoreView.h"

@implementation PlasmacoreView
- (void)handleDisplayLinkUpdate
{
  @synchronized ([Plasmacore singleton])
  {
    [super handleDisplayLinkUpdate];
  }
}

- (void)onCreate
{
  NSLog( @"PlasmacoreView '%@' created\n", [self valueForKey:@"name"] );
}

- (void)onDraw
{
  // Override as desired
//printf( "+onDraw\n" );
  PlasmacoreMessage* m_draw = [PlasmacoreMessage messageWithType:"Window.draw"];
  [m_draw setInt32:"window_id" value:[[Plasmacore singleton] idOfResource:self.window.windowController]];
  [m_draw setString:"view_name" value:[self valueForKey:@"name"] ];
  [m_draw setInt32:"width"  value:(int)[self frame].size.width];
  [m_draw setInt32:"height" value:(int)[self frame].size.height];
  [m_draw push];
//printf( "-onDraw\n" );

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


