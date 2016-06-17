//
//  PlasmacoreViewController-iOS.swift
//  iOS-Plasmacore
//
//  Created by Abraham Pralle on 5/11/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import GLKit
import OpenGLES

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
}


