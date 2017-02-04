//
//  AppDelegate.swift
//  Project-macOS
//
//  Created by Abraham Pralle on 11/27/16.
//  Copyright © 2016 Plasmaworks. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

  @IBOutlet weak var window: NSWindow!


  func applicationDidFinishLaunching(_ aNotification: Notification) {
    // Insert code here to initialize your application
    Plasmacore.onLaunch()
  }

  func applicationWillTerminate(_ aNotification: Notification) {
    // Insert code here to tear down your application
    Plasmacore.onSave()
    Plasmacore.onStop()
  }

  func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool
  {
    return true
  }
}

