//
//  PlasmacoreViewController-iOS.swift
//  iOS-Plasmacore
//
//  Created by Abraham Pralle on 5/11/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import GLKit
import OpenGLES

#if os(OSX)
    import Cocoa
#else
    import Foundation
    import UIKit
#endif


class PlasmacoreViewController : GLKViewController
{
  var context: EAGLContext? = nil
  var name = "Unnamed"


  deinit {
    self.tearDownGL()

    if EAGLContext.currentContext() === self.context
    {
      EAGLContext.setCurrentContext(nil)
    }
  }

  override func viewDidLoad()
  {
    super.viewDidLoad()

    self.context = EAGLContext(API: .OpenGLES2)

    if !(self.context != nil)
    {
      print("Failed to create ES context")
    }

    let view = self.view as! GLKView
    view.context = self.context!
    view.drawableDepthFormat = .Format24
    view.multipleTouchEnabled = true

    self.setupGL()
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()

    if self.isViewLoaded() && (self.view.window != nil)
    {
      self.view = nil

      self.tearDownGL()

      if EAGLContext.currentContext() === self.context
      {
        EAGLContext.setCurrentContext(nil)
      }
      self.context = nil
    }
  }

  func setupGL()
  {
    EAGLContext.setCurrentContext( self.context )
    preferredFramesPerSecond = 60
  }

  func tearDownGL()
  {
    EAGLContext.setCurrentContext(self.context)
  }

  func update()
  {
  }

  override func prefersStatusBarHidden()->Bool
  {
    return true
  }

  override func glkView( view:GLKView, drawInRect rect:CGRect)
  {
    let m = PlasmacoreMessage( type:"Display.render" )

    let scale = UIScreen.mainScreen().scale
    let display_width  = Int( UIScreen.mainScreen().bounds.size.width * scale )
    let display_height = Int( UIScreen.mainScreen().bounds.size.height * scale )

    m.set( "display_name",   value:name )
    m.set( "display_width",  value:Int(display_width) )
    m.set( "display_height", value:Int(display_height) )
    m.send()
  }

  override func touchesBegan( touches:Set<UITouch>, withEvent event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( "display_name",   value:name )
      m.set( "type", value:1 )  // press

      let scale = UIScreen.mainScreen().scale
      let os_pt = touch.locationInView( nil )
      m.set( "x",      value:Int( os_pt.x * scale ) )
      m.set( "y",      value:Int( os_pt.y * scale ) )
      m.set( "index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesMoved( touches:Set<UITouch>, withEvent event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( "display_name",   value:name )
      m.set( "type", value:0 )  // move

      let scale = UIScreen.mainScreen().scale
      let os_pt = touch.locationInView( nil )
      m.set( "x",      value:Int( os_pt.x * scale ) )
      m.set( "y",      value:Int( os_pt.y * scale ) )
      m.set( "index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesEnded( touches:Set<UITouch>, withEvent event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( "display_name",   value:name )
      m.set( "type", value:2 )  // release

      let scale = UIScreen.mainScreen().scale
      let os_pt = touch.locationInView( nil )
      m.set( "x",      value:Int( os_pt.x * scale ) )
      m.set( "y",      value:Int( os_pt.y * scale ) )
      m.set( "index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesCancelled( touches:Set<UITouch>?, withEvent event:UIEvent? )
  {
    guard let touches = touches else { return }

    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( "display_name",   value:name )
      m.set( "type", value:2 )  // release (we count canceled touches as releases)

      let scale = UIScreen.mainScreen().scale
      let os_pt = touch.locationInView( nil )
      m.set( "x",      value:Int( os_pt.x * scale ) )
      m.set( "y",      value:Int( os_pt.y * scale ) )
      m.set( "index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }
}


