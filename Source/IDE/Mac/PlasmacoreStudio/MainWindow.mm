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
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
  //NSLog( @"testView exists: %d\n", (self.view != nil));
  //printf( "HERE I AM\n" );
  
  
  //SEL method = NSSelectorFromString(@”name”);
  //id val = [myObj performSelector:method withObject:@"param"];

  NSLog( @"view has name: %d\n", [self.view respondsToSelector:NSSelectorFromString(@"name")] );

  //IMP method = [self.view methodForSelector:NSSelectorFromString(@"name")];
  SEL selector = NSSelectorFromString(@"name");
  NSLog( @"view's name is... %@\n", 
    ((id (*)(id, SEL))[self.view methodForSelector:selector])(self.view, selector)
  );
  NSLog( @"view's name 2 is %@\n", [self.view valueForKey:@"name"] );

  u_int property_count;
  objc_property_t* properties = class_copyPropertyList( [self class], &property_count );
  //NSMutableArray* property_array = [NSMutableArray arrayWithCapacity:property_count];
  for (int i = 0; i < property_count ; i++)
  {
    const char* property_name = property_getName( properties[i] );
    NSLog( @"Window has property named %s\n", property_name );
    //[propertyArray addObject:[NSString  stringWithCString:propertyName encoding:NSUTF8StringEncoding]];
  }
  free(properties);
}

@end
