//
//  PlasmacoreView.swift
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/27/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import Cocoa
import GLKit

class PlasmacoreView: NSOpenGLView
{
  var isConfigured = false
  var name         = "unnamed"
  var windowID     : Int?
  var displayLink  : CVDisplayLink?

  required init?(coder: NSCoder)
  {
    super.init( coder:coder )

    //  Create a pixel format and context and set them to the view's pixelFormat and openGLContext properties.
    let attributes: [NSOpenGLPixelFormatAttribute] = [
      UInt32(NSOpenGLPFAAccelerated),
      UInt32(NSOpenGLPFAColorSize), UInt32(32),
      UInt32(NSOpenGLPFADoubleBuffer),
      UInt32(NSOpenGLPFAOpenGLProfile),
      UInt32(NSOpenGLProfileVersion3_2Core),
      UInt32(0)
    ]

    guard let pixelFormat = NSOpenGLPixelFormat(attributes: attributes) else { return }
    self.pixelFormat = pixelFormat

    guard let context = NSOpenGLContext(format: pixelFormat, shareContext: nil) else { return }
    self.openGLContext = context
  }

  deinit
  {
    stopDisplayLink()
  }

  func configure()
  {
    if (isConfigured) { return }
    isConfigured = true

    if (name == "unnamed")
    {
      NSLog( "ERROR: Unnamed PlasmacoreView.  Set a 'name' string attribute in the interface builder's User Defined Runtime Attributes section." )
    }

    if let window = window
    {
      NSLog( "window controller: \(window.windowController) " )
      windowID = Plasmacore.singleton.getResourceID( window.windowController )
      window.acceptsMouseMovedEvents = true
      window.makeFirstResponder( self )
    }

    NSLog( "PlasmacoreView \(name) created in Window \(windowID)\n" )
  }

  override func drawRect( bounds:NSRect )
  {
    super.drawRect( bounds )
    configure()

    guard let context = self.openGLContext else { return }

    context.makeCurrentContext()
    CGLLockContext( context.CGLContextObj )

    let value = Float(sin(1.00 * CACurrentMediaTime()))
    glClearColor( 0, 0, value, 1.0 )
    glClear(GLbitfield(GL_COLOR_BUFFER_BIT))

    CGLFlushDrawable( context.CGLContextObj )

    CGLUnlockContext( context.CGLContextObj )

    startDisplayLink()
  }

  func onRedraw()
  {
    guard let window = window else { return }

    if (window.visible)
    {
      drawRect( self.bounds )
    }
    else
    {
      stopDisplayLink()
    }
  }

  override func prepareOpenGL()
  {
    self.openGLContext?.setValues( [1], forParameter:.GLCPSwapInterval )
  }

  func startDisplayLink()
  {
    if (displayLink !== nil) { return }

    // Locally defined callback
    func callback( displayLink:CVDisplayLink, _ now:UnsafePointer<CVTimeStamp>,
        _ output_time:UnsafePointer<CVTimeStamp>, _ input_flags:CVOptionFlags, _ output_flags:UnsafeMutablePointer<CVOptionFlags>,
        _ view_pointer:UnsafeMutablePointer<Void>) -> CVReturn
    {
      let view = unsafeBitCast( view_pointer, PlasmacoreView.self )

      // Ensure only one callback happens at a time
      objc_sync_enter( view ); defer { objc_sync_exit(view) }   // @synchronized (view)

      view.onRedraw()

      return kCVReturnSuccess
    }

    CVDisplayLinkCreateWithActiveCGDisplays( &displayLink )
    CVDisplayLinkSetOutputCallback( displayLink!, callback, UnsafeMutablePointer<Void>(unsafeAddressOf(self)) )
    CVDisplayLinkStart( displayLink! )
  }

  func stopDisplayLink()
  {
    if (displayLink === nil) { return }

    CVDisplayLinkStop( displayLink! )
    displayLink = nil
  }
}

