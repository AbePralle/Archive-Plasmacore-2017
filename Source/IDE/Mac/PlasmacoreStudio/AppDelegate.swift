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
    /*
    let mesg = PlasmacoreMessage( type: "Test" )
    mesg.setString( "stuff", value: "Whatever" )
    NSLog( "type: \(mesg.type)" )
    NSLog( "id: \(mesg.message_id)" )
    NSLog( "stuff: \(mesg.getString("stuff"))" )
    */

    //print_args( Int32(Process.arguments.count), Process.arguments )
    //hello_c( "hi" )
  }

  func applicationWillTerminate( notification:NSNotification)
  {
    // Insert code here to tear down your application
  }
}

