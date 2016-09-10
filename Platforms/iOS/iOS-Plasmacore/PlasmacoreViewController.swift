//
//  GameViewController.swift
//  iOS-Plasmacore
//
//  Created by Abraham Pralle on 5/11/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import GLKit
import OpenGLES

class GameViewController: GLKViewController
{
  var context: EAGLContext? = nil

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
  }
}

