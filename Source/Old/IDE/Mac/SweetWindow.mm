//
//  SweetWindow.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/12/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "SweetWindow.h"
#import "Plasmacore.h"

@interface SweetWindow ()
@property (weak) IBOutlet NSTextField *results;

@end

@implementation SweetWindow


- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
  [[Plasmacore singleton] addListenerForType:"add_result" withCallback:
   ^(int THIS, PlasmacoreMessage* m)
   {
     [self.results setStringValue:[m getString:"color"]];
     NSLog( @"Got message\n" );
   }
   ];
}
- (IBAction)buttonClicked:(id)sender {
  NSLog( @"Button clicked\n" );
  [[[PlasmacoreMessage messageWithType:"some_button_clicked"] setString:"stuff" value:@"Whatever"] send];
}

@end
