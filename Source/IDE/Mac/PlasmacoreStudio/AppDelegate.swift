//
//  AppDelegate.swift
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/13/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import Cocoa
import Foundation

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate
{
  @IBOutlet weak var window: NSWindow!

  func applicationDidFinishLaunching( notification:NSNotification )
  {
    Plasmacore.singleton.configure().launch()
    Plasmacore.singleton.start()
  }

  func applicationShouldHandleReopen( sender:NSApplication, hasVisibleWindows:Bool )->Bool
  {
    if (hasVisibleWindows) { return true; }

    Plasmacore.singleton.relaunch();
    return false;
  }

  func applicationWillTerminate( notification:NSNotification)
  {
    Plasmacore.singleton.stop()
  }
}

