//
//  PlasmacoreView.swift
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/27/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import Cocoa

class PlasmacoreView: NSOpenGLView {

    override func drawRect(dirtyRect: NSRect) {
        super.drawRect(dirtyRect)

        // Drawing code here.
    }
    

    /*
    var displayLink: CVDisplayLink?

    required init?(coder: NSCoder) {

        //  Call the super before setting the pixelFormat and openGLContext so that the super does not override 
        //  our custom versions of these properties.
        super.init(coder: coder)

        //  Create a pixel format and context and set them to the view's pixelFormat and openGLContext properties.
        let attributes: [NSOpenGLPixelFormatAttribute] = [
            UInt32(NSOpenGLPFAAccelerated),
            UInt32(NSOpenGLPFAColorSize), UInt32(32),
            UInt32(NSOpenGLPFADoubleBuffer),
            UInt32(NSOpenGLPFAOpenGLProfile),
            UInt32(NSOpenGLProfileVersion3_2Core),
            UInt32(0)
        ]
        guard let pixelFormat = NSOpenGLPixelFormat(attributes: attributes) else {
            Swift.print("pixel format could not be created")
            return
        }
        self.pixelFormat = pixelFormat

        guard let context = NSOpenGLContext(format: pixelFormat, shareContext: nil) else {
            Swift.print("context could not be created")
            return
        }
        self.openGLContext = context

        //  Tell the view how often we are swaping the buffers, 1 indicates we are using the 60Hz refresh rate (i.e. 60 fps)
        self.openGLContext?.setValues([1], forParameter: .GLCPSwapInterval)

    }

    //  prepareOpenGL is where we set OpenGL state calls before the first render, we will set up the CVDisplayLink here.
    override func prepareOpenGL() {

        //  The callback function is called everytime CVDisplayLink says its time to get a new frame.
        func displayLinkOutputCallback(displayLink: CVDisplayLink, _ inNow: UnsafePointer<CVTimeStamp>, _ inOutputTime: UnsafePointer<CVTimeStamp>, _ flagsIn: CVOptionFlags, _ flagsOut: UnsafeMutablePointer<CVOptionFlags>, _ displayLinkContext: UnsafeMutablePointer<Void>) -> CVReturn {

            /*  The displayLinkContext is CVDisplayLink's parameter definition of the view in which we are working.
                In order to access the methods of a given view we need to specify what kind of view it is as right
                now the UnsafeMutablePointer<Void> just means we have a pointer to "something".  To cast the pointer
                such that the compiler at runtime can access the methods associated with our SwiftOpenGLView, we use
                an unsafeBitCast.  The definition of which states, "Returns the the bits of x, interpreted as having
                type U."  We may then call any of that view's methods.  Here we call drawView() which we draw a
                frame for rendering.  */
            unsafeBitCast(displayLinkContext, SwiftOpenGLView.self).renderFrame()

            //  We are going to assume that everything went well for this mock up, and pass success as the CVReturn
            return kCVReturnSuccess
        }

        //  Grab the a link to the active displays, set the callback defined above, and start the link.
        /*  An alternative to a nested function is a global function or a closure passed as the argument--a local function 
            (i.e. a function defined within the class) is NOT allowed. */
        //  The UnsafeMutablePointer<Void>(unsafeAddressOf(self)) passes a pointer to the instance of our class.
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink)
        CVDisplayLinkSetOutputCallback(displayLink!, displayLinkOutputCallback, UnsafeMutablePointer<Void>(unsafeAddressOf(self)))
        CVDisplayLinkStart(displayLink!)

    }

    //  Method called to render a new frame with an OpenGL pipeline
    func renderFrame() {

        guard let context = self.openGLContext else {
            Swift.print("oops")
            return
        }

        //  Tell OpenGL this is the context we want to draw into and lock the focus.
        context.makeCurrentContext()
        CGLLockContext(context.CGLContextObj)

        //  Lock the focus before making state change calls to OpenGL, or the app gives you a EXC_BAD_ACCESS fault
        //  This float is a changing value we can use to create a simple animation.
        let value = Float(sin(1.00 * CACurrentMediaTime()))
        //  Uses the float to set a clear color that is on the gray scale.
        glClearColor(value, value, value, 1.0)

        glClear(GLbitfield(GL_COLOR_BUFFER_BIT))
        //  Flushing sends the context to be used for display, then we can unlock the focus.
        CGLFlushDrawable(context.CGLContextObj)
        CGLUnlockContext(context.CGLContextObj)

    }

    override func drawRect(dirtyRect: NSRect) {

        super.drawRect(dirtyRect)

        // Should drawRect(_:) get called, we want a new frame to be drawn, so call drawView()
        renderFrame()

    }

    deinit {

        //When the view gets destroyed, we don't want to keep the link going.
        CVDisplayLinkStop(displayLink!)

    }
    */
}
