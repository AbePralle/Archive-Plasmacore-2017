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

    var mesg = PlasmacoreMessage( type: "Test" )
    mesg.setString( "stuff", value: "Whatever" )
    NSLog( "type: \(mesg.type)" )
    NSLog( "id: \(mesg.message_id)" )
    NSLog( "stuff: \(mesg.getString("stuff"))" )
    mesg.send()

    mesg = PlasmacoreMessage( type:"AnotherMessage" )
    mesg.setInt32( "hex", value:0x11223344 )
    mesg.setReal64( "pi", value:3.1415926 )
    mesg.send()

    //print_args( Int32(Process.arguments.count), Process.arguments )
    //hello_c( "hi" )
  }

  func applicationWillTerminate( notification:NSNotification)
  {
    // Insert code here to tear down your application
  }
}

