//
//  MainWindow.m
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 2/20/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import <objc/runtime.h>
#import "MainWindow.h"

@interface MainWindow ()
@property (weak) IBOutlet NSView *view;

@end

@implementation MainWindow

- (void)windowDidLoad {
    [super windowDidLoad];
}

- (void)bam:(CCMessage*)m
{
  NSLog( @"BAM! (method name %@)\n", [m getString:"method_name"] );
}

- (void)pow:(CCMessage*)m
{
  printf( "POW!\n" );
}

@end
