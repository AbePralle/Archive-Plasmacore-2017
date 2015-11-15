//
//  PlasmaGLView.swift
//  Plasmacore
//
//  Created by Abraham Pralle on 6/15/15.
//  Copyright (c) 2015 Plasmaworks. All rights reserved.
//

import Foundation
import AppKit
import GLKit

class PlasmaGLView : NSOpenGLView
{
  var view : PlasmaGLView?
  
  override func awakeFromNib()
  {
    var st : String?
    st = "hello"
    if let st = st
    {
      NSLog( "Here I am!" )
    }
    else
    {
      NSLog( "There you are!" )
    }
    //while let st = st {
      //st.hashValue
    //}
    /*
    var attributes : [NSOpenGLPixelFormatAttribute] =
    [
      NSOpenGLPixelFormatAttribute(NSOpenGLPFADepthSize),
      NSOpenGLPixelFormatAttribute(24),
      NSOpenGLPixelFormatAttribute(NSOpenGLPFAOpenGLProfile),
      NSOpenGLPixelFormatAttribute(NSOpenGLProfileVersion3_2Core),
      NSOpenGLPixelFormatAttribute(0)
    ]
    
    self.pixelFormat = NSOpenGLPixelFormat(attributes:attributes)
*/
  }
  
  override func prepareOpenGL()
  {
    glClearColor( 1.0, 0.0, 0.0, 1.0 )
  }
  
  override func drawRect( rect:NSRect )
  {
    prepareOpenGL()
    glClear( GLbitfield(GL_COLOR_BUFFER_BIT) )
    glFlush()
  }
}
