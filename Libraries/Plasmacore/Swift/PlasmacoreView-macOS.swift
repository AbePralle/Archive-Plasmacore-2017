//
//  PlasmacoreView.swift
//  PlasmacoreStudio
//
//  Created by Abraham Pralle on 3/27/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#if os(OSX)
import Cocoa
import AppKit

import GLKit

class PlasmacoreView: NSOpenGLView
{

  var isConfigured = false
  var name         = "unnamed"
  var windowID     = 0
  var displayLink  : CVDisplayLink?
  var keyModifierFlags:UInt = 0
  var renderer_id:Int32 = 0

  var view_bounds  = NSRect(x:0,y:0,width:1,height:1)

  required init?(coder: NSCoder)
  {
    super.init( coder:coder )

    //  Create a pixel format and context and set them to the view's pixelFormat and openGLContext properties.
    let attributes: [NSOpenGLPixelFormatAttribute] = [
      UInt32(NSOpenGLPFAAccelerated),
      UInt32(NSOpenGLPFAColorSize), UInt32(32),
      UInt32(NSOpenGLPFADoubleBuffer),
      UInt32(NSOpenGLPFAOpenGLProfile),
      UInt32(NSOpenGLProfileVersionLegacy),
      UInt32(0)
    ]

    guard let pixelFormat = NSOpenGLPixelFormat(attributes: attributes) else { return }
    self.pixelFormat = pixelFormat

    guard let context = NSOpenGLContext(format: pixelFormat, share: nil) else { return }
    self.openGLContext = context
  }

  deinit
  {
    stopDisplayLink()
  }

  override func becomeFirstResponder() -> Bool
  {
    if ( !super.becomeFirstResponder() ) { return false }
    configure()
    let m = PlasmacoreMessage( type:"Display.focus_gained" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.send()
    return true
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
      NSLog( "window controller: \(String(describing:window.windowController)) " )
      windowID = Plasmacore.singleton.getResourceID( window.windowController )
      window.acceptsMouseMovedEvents = true
      window.makeFirstResponder( self )
    }

    NSLog( "PlasmacoreView \(name) created in Window \(windowID)\n" )
  }

  override func draw( _ area:NSRect )
  {
    super.draw( area )
    view_bounds = self.bounds

    guard let window = window else { return }
    if (window.isVisible)
    {
      commonDraw()
    }
    else
    {
      stopDisplayLink()
    }
  }

  func commonDraw()
  {
    configure()

    guard let context = self.openGLContext else { return }

    context.makeCurrentContext()
    CGLLockContext( context.cglContextObj! )


    let display_width  = Int( view_bounds.width )
    let display_height = Int( view_bounds.height )

    let m = PlasmacoreMessage( type:"Display.on_render" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"display_width",  value:display_width )
    m.set( name:"display_height", value:display_height )
    m.send()

    CGLFlushDrawable( context.cglContextObj! )
    CGLUnlockContext( context.cglContextObj! )

    startDisplayLink()
  }

  override func flagsChanged( with event:NSEvent )
  {
    let newFlags = event.modifierFlags.rawValue
    let modified = keyModifierFlags ^ newFlags
    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.shift.rawValue, keycode:Keycode.CAPS_LOCK )

    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.shift.rawValue|2, keycode:Keycode.LEFT_SHIFT )
    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.shift.rawValue|4, keycode:Keycode.RIGHT_SHIFT )

    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.control.rawValue|1,      keycode:Keycode.LEFT_CONTROL )
    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.control.rawValue|0x2000, keycode:Keycode.RIGHT_CONTROL )

    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.option.rawValue|0x20, keycode:Keycode.LEFT_ALT )
    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.option.rawValue|0x40, keycode:Keycode.RIGHT_ALT )

    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.command.rawValue|0x08, keycode:Keycode.LEFT_OS )
    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.command.rawValue|0x10, keycode:Keycode.RIGHT_OS )

    handleModifiedKey( modified:modified, mask:NSEventModifierFlags.numericPad.rawValue, keycode:Keycode.NUMPAD_ENTER )

    keyModifierFlags = newFlags
  }

  func handleModifiedKey( modified:UInt, mask:UInt, keycode:Int )
  {
    configure()
    if ((modified & mask) == mask)
    {
      let m = PlasmacoreMessage( type:"Display.on_key_event" )
      m.set( name:"window_id", value:windowID )
      m.set( name:"display_name", value:name )
      m.set( name:"is_press", value:(keyModifierFlags & mask) == 0 )
      m.set( name:"keycode", value:keycode )
      m.send()
    }
  }

  override func keyDown( with event:NSEvent )
  {
    configure()
    let syscode = Int( event.keyCode & 0x7f )
    let keycode = syscodeToKeycode[ syscode ]

    var m = PlasmacoreMessage( type:"Display.on_key_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"is_press", value:true )
    m.set( name:"keycode", value:keycode )
    m.set( name:"syscode", value:syscode )
    m.set( name:"is_repeat", value:event.isARepeat )
    m.send()

    guard let characters = event.characters else { return }
    guard characters.characters.count > 0 else { return }
    let unicode = Int( characters.unicodeScalars[ characters.unicodeScalars.startIndex ].value )

    // Don't send unicode 0..31 or 127 as a TextEvent
    if (characters.characters.count > 1 || (characters.characters.count == 1 && unicode >= 32 && unicode != 127))
    {
      m = PlasmacoreMessage( type:"Display.on_text_event" )
      m.set( name:"window_id", value:windowID )
      m.set( name:"display_name", value:name )
      m.set( name:"character", value:unicode )

      if (characters.characters.count > 1)
      {
        m.set( name:"text", value:characters )
      }
      m.send()
    }
  }

  override func keyUp( with event:NSEvent )
  {
    configure()
    let syscode = Int( event.keyCode & 0x7f )
    let keycode = syscodeToKeycode[ syscode ]

    let m = PlasmacoreMessage( type:"Display.on_key_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"is_press", value:false )
    m.set( name:"keycode", value:keycode )
    m.set( name:"syscode", value:syscode )
    m.send()
  }

  override func mouseDown( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from:nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:1 )  // 1=press
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.set( name:"index", value:0 )
    m.send()
  }

  override func mouseDragged( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:0 )  // 0=move
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.send()
  }

  override func mouseMoved( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:0 )  // 0=move
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.send()
  }

  override func mouseUp( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:2 )  // 2=release
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.set( name:"index", value:0 )
    m.send()
  }

  func onRedraw()
  {
    commonDraw()
  }

  override func prepareOpenGL()
  {
    self.openGLContext?.setValues( [1], for:NSOpenGLCPSwapInterval )
  }

  func startDisplayLink()
  {
    if (displayLink !== nil) { return }
    NSLog( "start display link" )

    // Locally defined callback
    func callback( displayLink:CVDisplayLink, now:UnsafePointer<CVTimeStamp>,
        output_time:UnsafePointer<CVTimeStamp>, input_flags:CVOptionFlags, output_flags:UnsafeMutablePointer<CVOptionFlags>,
        view_pointer:UnsafeMutableRawPointer?) -> CVReturn
    {
      guard let view_pointer = view_pointer else { return kCVReturnSuccess }
      let view = unsafeBitCast( view_pointer, to:PlasmacoreView.self )

      // Ensure only one callback happens at a time
      objc_sync_enter( view ); defer { objc_sync_exit(view) }   // @synchronized (view)

      view.onRedraw()

      return kCVReturnSuccess
    }

    CVDisplayLinkCreateWithActiveCGDisplays( &displayLink )
    CVDisplayLinkSetOutputCallback( displayLink!, callback, UnsafeMutableRawPointer(mutating: Unmanaged.passUnretained(self).toOpaque()) )
    CVDisplayLinkStart( displayLink! )
  }

  override func rightMouseDown( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:1 )  // 1=press
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.set( name:"index", value:1 )
    m.send()
  }

  override func rightMouseDragged( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:0 )  // 0=move
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.send()
  }

  override func rightMouseUp( with event:NSEvent )
  {
    configure()
    let point = convert( event.locationInWindow, from: nil )

    let m = PlasmacoreMessage( type:"Display.on_pointer_event" )
    m.set( name:"window_id", value:windowID )
    m.set( name:"display_name", value:name )
    m.set( name:"type", value:2 )  // 2=release
    m.set( name:"x", value:Double(point.x) )
    m.set( name:"y", value:Double(bounds.size.height - point.y) )
    m.set( name:"index", value:1 )
    m.send()
  }

  override func scrollWheel( with event:NSEvent )
  {
    configure()
    let dx = event.deltaX
    let dy = event.deltaY

    if (dx >= 0.0001 || dx <= -0.0001 || dy >= 0.0001 || dy <= -0.0001)
    {
      let m = PlasmacoreMessage( type:"Display.on_scroll_event" )
      m.set( name:"window_id", value:windowID )
      m.set( name:"display_name", value:name )
      m.set( name:"dx", value:Double(dx) )
      m.set( name:"dy", value:Double(dy) )
      m.send()
    }
  }

  func stopDisplayLink()
  {
    if (displayLink === nil) { return }

    CVDisplayLinkStop( displayLink! )
    displayLink = nil
  }

  struct KeyModifier
  {
    static let SHIFT     =  1
    static let CTRL      =  2
    static let ALT       =  4
    static let CAPS_LOCK =  8
    static let COMMAND   = 16
  }

  struct Keycode
  {
    static let LEFT_ARROW       =  1
    static let UP_ARROW         =  2
    static let RIGHT_ARROW      =  3
    static let DOWN_ARROW       =  4

    static let BACKSPACE        =  8
    static let TAB              =  9
    static let ENTER            = 10

    static let CAPS_LOCK        = 11
    static let LEFT_SHIFT       = 12
    static let RIGHT_SHIFT      = 13
    static let LEFT_CONTROL     = 14
    static let RIGHT_CONTROL    = 15
    static let LEFT_ALT         = 16
    static let RIGHT_ALT        = 17

    // Windows/Command key
    static let LEFT_OS          = 18
    static let RIGHT_OS         = 19

    static let FUNCTION         = 26
    static let ESCAPE           = 27

    static let SPACE            = 32

    static let APOSTROPHE       = 39
    static let COMMA            = 44
    static let MINUS            = 45
    static let PERIOD           = 46
    static let SLASH            = 47
    static let DIGIT_0          = 48
    static let DIGIT_1          = 49
    static let DIGIT_2          = 50
    static let DIGIT_3          = 51
    static let DIGIT_4          = 52
    static let DIGIT_5          = 53
    static let DIGIT_6          = 54
    static let DIGIT_7          = 55
    static let DIGIT_8          = 56
    static let DIGIT_9          = 57
    static let SEMICOLON        = 59
    static let EQUALS           = 61

    static let A                = 65
    static let B                = 66
    static let C                = 67
    static let D                = 68
    static let E                = 69
    static let F                = 70
    static let G                = 71
    static let H                = 72
    static let I                = 73
    static let J                = 74
    static let K                = 75
    static let L                = 76
    static let M                = 77
    static let N                = 78
    static let O                = 79
    static let P                = 80
    static let Q                = 81
    static let R                = 82
    static let S                = 83
    static let T                = 84
    static let U                = 85
    static let V                = 86
    static let W                = 87
    static let X                = 88
    static let Y                = 89
    static let Z                = 90

    static let OPEN_BRACKET     = 91
    static let BACKSLASH        = 92
    static let CLOSE_BRACKET    = 93
    static let BACKQUOTE        = 96

    static let NUMPAD_ENTER     = 110

    static let SYS_REQUEST      = 124
    static let SCROLL_LOCK      = 125
    static let BREAK            = 126

    static let DELETE           = 127
    static let INSERT           = 128
    static let HOME             = 129
    static let END              = 130
    static let PAGE_UP          = 131
    static let PAGE_DOWN        = 132

    static let NUMPAD_ASTERISK  = 142
    static let NUMPAD_PLUS      = 143
    static let NUMPAD_MINUS     = 145
    static let NUMPAD_PERIOD    = 146
    static let NUMPAD_SLASH     = 147
    static let NUMPAD_0         = 148
    static let NUMPAD_1         = 149
    static let NUMPAD_2         = 150
    static let NUMPAD_3         = 151
    static let NUMPAD_4         = 152
    static let NUMPAD_5         = 153
    static let NUMPAD_6         = 154
    static let NUMPAD_7         = 155
    static let NUMPAD_8         = 156
    static let NUMPAD_9         = 157
    static let NUMPAD_NUM_LOCK  = 158
    static let NUMPAD_EQUALS    = 161

    static let F1               = 201
    static let F2               = 202
    static let F3               = 203
    static let F4               = 204
    static let F5               = 205
    static let F6               = 206
    static let F7               = 207
    static let F8               = 208
    static let F9               = 209
    static let F10              = 210
    static let F11              = 211
    static let F12              = 212
    static let F13              = 213
    static let F14              = 214
    static let F15              = 215
    static let F16              = 216
    static let F17              = 217
    static let F18              = 218
    static let F19              = 219
  }

  let syscodeToKeycode =
  [
    Keycode.A,               //   0 -> a
    Keycode.S,               //   1 -> s
    Keycode.D,               //   2 -> d
    Keycode.F,               //   3 -> f
    Keycode.H,               //   4 -> h
    Keycode.G,               //   5 -> g
    Keycode.Z,               //   6 -> z
    Keycode.X,               //   7 -> x
    Keycode.C,               //   8 -> c
    Keycode.V,               //   9 -> v
    0,                       //  10 -> (unknown)
    Keycode.B,               //  11 -> b
    Keycode.Q,               //  12 -> q
    Keycode.W,               //  13 -> w
    Keycode.E,               //  14 -> e
    Keycode.R,               //  15 -> r
    Keycode.Y,               //  16 -> y
    Keycode.T,               //  17 -> t
    Keycode.DIGIT_1,         //  18 -> 1
    Keycode.DIGIT_2,         //  19 -> 2
    Keycode.DIGIT_3,         //  20 -> 3
    Keycode.DIGIT_4,         //  21 -> 4
    Keycode.DIGIT_6,         //  22 -> 6
    Keycode.DIGIT_5,         //  23 -> 5
    Keycode.EQUALS,          //  24 -> EQUALS
    Keycode.DIGIT_9,         //  25 -> 9
    Keycode.DIGIT_7,         //  26 -> 7
    Keycode.MINUS,           //  27 -> MINUS
    Keycode.DIGIT_8,         //  28 -> 8
    Keycode.DIGIT_0,         //  29 -> 0
    Keycode.CLOSE_BRACKET,   //  30 -> ]
    Keycode.O,               //  31 -> o
    Keycode.U,               //  32 -> u
    Keycode.OPEN_BRACKET,    //  33 -> [
    Keycode.I,               //  34 -> i
    Keycode.P,               //  35 -> p
    Keycode.ENTER,           //  36 -> ENTER
    Keycode.L,               //  37 -> l
    Keycode.J,               //  38 -> j
    Keycode.APOSTROPHE,      //  39 -> '
    Keycode.K,               //  40 -> k
    Keycode.SEMICOLON,       //  41 -> ;
    Keycode.BACKSLASH,       //  42 -> '\'
    Keycode.COMMA,           //  43 -> ,
    Keycode.SLASH,           //  44 -> /
    Keycode.N,               //  45 -> n
    Keycode.M,               //  46 -> m
    Keycode.PERIOD,          //  47 -> .
    Keycode.TAB,             //  48 -> TAB
    Keycode.SPACE,           //  49 -> SPACE
    Keycode.BACKQUOTE,       //  50 -> grave (back quote)
    Keycode.BACKSPACE,       //  51 -> BACKSPACE
    0,                       //  52 -> (unknown)
    Keycode.ESCAPE,          //  53 -> ESC
    0,                       //  54 -> (unknown)
    0,                       //  55 -> (unknown)
    0,                       //  56 -> (unknown)
    0,                       //  57 -> (unknown)
    0,                       //  58 -> (unknown)
    0,                       //  59 -> (unknown)
    0,                       //  60 -> (unknown)
    0,                       //  61 -> (unknown)
    0,                       //  62 -> (unknown)
    0,                       //  63 -> (unknown)
    Keycode.F17,             //  64 -> F17
    Keycode.NUMPAD_PERIOD,   //  65 -> NUMPAD .
    0,                       //  66 -> (unknown)
    Keycode.NUMPAD_ASTERISK, //  67 -> NUMPAD *
    0,                       //  68 -> (unknown)
    Keycode.NUMPAD_PLUS,     //  69 -> NUMPAD +
    0,                       //  70 -> (unknown)
    Keycode.NUMPAD_NUM_LOCK, //  71 -> CLEAR (Mac)
    0,                       //  72 -> (unknown)
    0,                       //  73 -> (unknown)
    0,                       //  74 -> (unknown)
    Keycode.SLASH,           //  75 -> NUMPAD /
    0,                       //  76 -> (unknown)
    0,                       //  77 -> (unknown)
    Keycode.NUMPAD_MINUS,    //  78 -> NUMPAD -
    Keycode.F18,             //  79 -> F18
    Keycode.F19,             //  80 -> F19
    Keycode.NUMPAD_EQUALS,   //  81 -> NUMPAD =
    Keycode.NUMPAD_0,        //  82 -> NUMPAD 0
    Keycode.NUMPAD_1,        //  83 -> NUMPAD 1
    Keycode.NUMPAD_2,        //  84 -> NUMPAD 2
    Keycode.NUMPAD_3,        //  85 -> NUMPAD 3
    Keycode.NUMPAD_4,        //  86 -> NUMPAD 4
    Keycode.NUMPAD_5,        //  87 -> NUMPAD 5
    Keycode.NUMPAD_6,        //  88 -> NUMPAD 6
    Keycode.NUMPAD_7,        //  89 -> NUMPAD 7
    0,                       //  90 -> (unknown)
    Keycode.NUMPAD_8,        //  91 -> NUMPAD 8
    Keycode.NUMPAD_9,        //  92 -> NUMPAD 9
    0,                       //  93 -> (unknown)
    0,                       //  94 -> (unknown)
    0,                       //  95 -> (unknown)
    Keycode.F5,              //  96 -> F5
    Keycode.F6,              //  97 -> F6
    Keycode.F7,              //  98 -> F7
    Keycode.F3,              //  99 -> F3
    Keycode.F8,              // 100 -> F8
    Keycode.F9,              // 101 -> F9
    0,                       // 102 -> (unknown)
    Keycode.F11,             // 103 -> F11
    0,                       // 104 -> (unknown)
    Keycode.F13,             // 105 -> F13
    Keycode.F16,             // 106 -> F16
    Keycode.F14,             // 107 -> F14
    0,                       // 108 -> (unknown)
    Keycode.F10,             // 109 -> F10
    0,                       // 110 -> (unknown)
    Keycode.F12,             // 111 -> F12
    0,                       // 112 -> (unknown)
    Keycode.F15,             // 113 -> F15
    0,                       // 114 -> (unknown)
    Keycode.HOME,            // 115 -> HOME
    Keycode.PAGE_UP,         // 116 -> PAGE UP
    Keycode.DELETE,          // 117 -> DELETE
    Keycode.F4,              // 118 -> F4
    Keycode.END,             // 119 -> END
    Keycode.F2,              // 120 -> F2
    Keycode.PAGE_DOWN,       // 121 -> PAGE DOWN
    Keycode.F1,              // 122 -> F1
    Keycode.LEFT_ARROW,      // 123 -> LEFT ARROW
    Keycode.RIGHT_ARROW,     // 124 -> RIGHT ARROW
    Keycode.DOWN_ARROW,      // 125 -> DOWN ARROW
    Keycode.UP_ARROW,        // 126 -> UP ARROW
    0                           // 127 -> (unknown)
  ]
}

#endif
// os(OSX)
