//
//  AppDelegate.swift
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate
{
  @IBOutlet weak var window: NSWindow!

  func applicationDidFinishLaunching( notification:NSNotification )
  {
    Plasmacore.singleton.sayHello();

    hello_c( "What are the haps my friends?".cStringUsingEncoding(NSUTF8StringEncoding) );

    // Insert code here to initialize your application
  }

  func applicationWillTerminate( notification:NSNotification)
  {
    // Insert code here to tear down your application
  }
}

