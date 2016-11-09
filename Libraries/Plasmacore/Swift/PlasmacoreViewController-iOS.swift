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
  var name = "Main"
  var context: EAGLContext? = nil


  deinit {
    self.tearDownGL()

    if EAGLContext.current() === self.context
    {
      EAGLContext.setCurrent(nil)
    }
  }

  override func viewDidLoad()
  {
    super.viewDidLoad()

    self.context = EAGLContext(api: .openGLES2)

    if !(self.context != nil)
    {
      print("Failed to create ES context")
    }

    let view = self.view as! GLKView
    view.context = self.context!
    view.drawableDepthFormat = .format24
    view.isMultipleTouchEnabled = true

    self.setupGL()
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()

    if self.isViewLoaded && (self.view.window != nil)
    {
      self.view = nil

      self.tearDownGL()

      if EAGLContext.current() === self.context
      {
        EAGLContext.setCurrent(nil)
      }
      self.context = nil
    }
  }

  func setupGL()
  {
    EAGLContext.setCurrent( self.context )
    preferredFramesPerSecond = 60
  }

  func tearDownGL()
  {
    EAGLContext.setCurrent(self.context)
  }

  func update()
  {
  }

  override var prefersStatusBarHidden:Bool
  {
    return true
  }

  override func glkView( _ view:GLKView, drawIn rect:CGRect)
  {
    let scale = UIScreen.main.scale
    let display_width  = Int( view.bounds.size.width * scale )
    let display_height = Int( view.bounds.size.height * scale )

    if (display_width == 0 || display_height == 0) { return }

    let m = PlasmacoreMessage( type:"Display.render" )
    m.set( name:"display_name",   value:name )
    m.set( name:"display_width",  value:Int(display_width) )
    m.set( name:"display_height", value:Int(display_height) )

    // viewport size only differs from display size on the iPhone Plus physical device (not simulator)
    m.set( name:"viewport_width",  value:Int(UIScreen.main.nativeBounds.width))
    m.set( name:"viewport_height", value:Int(UIScreen.main.nativeBounds.height))
    m.send()
  }

  override func touchesBegan( _ touches:Set<UITouch>, with event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( name:"display_name",   value:name )
      m.set( name:"type", value:1 )  // press

      let scale = UIScreen.main.scale
      let os_pt = touch.location( in: self.view )
      m.set( name:"x",      value:Int( os_pt.x * scale ) )
      m.set( name:"y",      value:Int( os_pt.y * scale ) )
      m.set( name:"index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesMoved( _ touches:Set<UITouch>, with event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( name:"display_name",   value:name )
      m.set( name:"type", value:0 )  // move

      let scale = UIScreen.main.scale
      let os_pt = touch.location( in: self.view )
      m.set( name:"x",      value:Int( os_pt.x * scale ) )
      m.set( name:"y",      value:Int( os_pt.y * scale ) )
      m.set( name:"index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesEnded( _ touches:Set<UITouch>, with event:UIEvent? )
  {
    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( name:"display_name",   value:name )
      m.set( name:"type", value:2 )  // release

      let scale = UIScreen.main.scale
      let os_pt = touch.location( in: self.view )
      m.set( name:"x",      value:Int( os_pt.x * scale ) )
      m.set( name:"y",      value:Int( os_pt.y * scale ) )
      m.set( name:"index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }

  override func touchesCancelled( _ touches:Set<UITouch>?, with event:UIEvent? )
  {
    guard let touches = touches else { return }

    var touch_index = 0
    for touch in touches
    {
      let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
      m.set( name:"display_name",   value:name )
      m.set( name:"type", value:2 )  // release (we count canceled touches as releases)

      let scale = UIScreen.main.scale
      let os_pt = touch.location( in: self.view )
      m.set( name:"x",      value:Int( os_pt.x * scale ) )
      m.set( name:"y",      value:Int( os_pt.y * scale ) )
      m.set( name:"index",  value:touch_index )
      m.send()

      touch_index += 1
    }
  }
}


