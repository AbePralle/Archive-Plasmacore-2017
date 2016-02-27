#import  "Plasmacore.h"
#import  "PlasmacoreView.h"

#include "SuperCPPCString.h"

#define PC_KEY_MODIFIER_SHIFT        1
#define PC_KEY_MODIFIER_CTRL         2
#define PC_KEY_MODIFIER_ALT          4
#define PC_KEY_MODIFIER_CAPS_LOCK    8
#define PC_KEY_MODIFIER_COMMAND     16

#define PC_KEYCODE_LEFT_ARROW        1
#define PC_KEYCODE_UP_ARROW          2
#define PC_KEYCODE_RIGHT_ARROW       3
#define PC_KEYCODE_DOWN_ARROW        4

#define PC_KEYCODE_BACKSPACE         8
#define PC_KEYCODE_TAB               9
#define PC_KEYCODE_ENTER            10

#define PC_KEYCODE_CAPS_LOCK        11
#define PC_KEYCODE_LEFT_SHIFT       12
#define PC_KEYCODE_RIGHT_SHIFT      13
#define PC_KEYCODE_LEFT_CONTROL     14
#define PC_KEYCODE_RIGHT_CONTROL    15
#define PC_KEYCODE_LEFT_ALT         16
#define PC_KEYCODE_RIGHT_ALT        17

// Windows/Command key
#define PC_KEYCODE_LEFT_OS          18
#define PC_KEYCODE_RIGHT_OS         19

#define PC_KEYCODE_FUNCTION         26
#define PC_KEYCODE_ESCAPE           27

#define PC_KEYCODE_SPACE            32

#define PC_KEYCODE_APOSTROPHE       39
#define PC_KEYCODE_COMMA            44
#define PC_KEYCODE_MINUS            45
#define PC_KEYCODE_PERIOD           46
#define PC_KEYCODE_SLASH            47
#define PC_KEYCODE_0                48
#define PC_KEYCODE_1                49
#define PC_KEYCODE_2                50
#define PC_KEYCODE_3                51
#define PC_KEYCODE_4                52
#define PC_KEYCODE_5                53
#define PC_KEYCODE_6                54
#define PC_KEYCODE_7                55
#define PC_KEYCODE_8                56
#define PC_KEYCODE_9                57
#define PC_KEYCODE_SEMICOLON        59
#define PC_KEYCODE_EQUALS           61

#define PC_KEYCODE_A                65
#define PC_KEYCODE_B                66
#define PC_KEYCODE_C                67
#define PC_KEYCODE_D                68
#define PC_KEYCODE_E                69
#define PC_KEYCODE_F                70
#define PC_KEYCODE_G                71
#define PC_KEYCODE_H                72
#define PC_KEYCODE_I                73
#define PC_KEYCODE_J                74
#define PC_KEYCODE_K                75
#define PC_KEYCODE_L                76
#define PC_KEYCODE_M                77
#define PC_KEYCODE_N                78
#define PC_KEYCODE_O                79
#define PC_KEYCODE_P                80
#define PC_KEYCODE_Q                81
#define PC_KEYCODE_R                82
#define PC_KEYCODE_S                83
#define PC_KEYCODE_T                84
#define PC_KEYCODE_U                85
#define PC_KEYCODE_V                86
#define PC_KEYCODE_W                87
#define PC_KEYCODE_X                88
#define PC_KEYCODE_Y                89
#define PC_KEYCODE_Z                90

#define PC_KEYCODE_OPEN_BRACKET     91
#define PC_KEYCODE_BACKSLASH        92
#define PC_KEYCODE_CLOSE_BRACKET    93
#define PC_KEYCODE_BACKQUOTE        96

#define PC_KEYCODE_NUMPAD_ENTER    110

#define PC_KEYCODE_SYS_REQUEST     124
#define PC_KEYCODE_SCROLL_LOCK     125
#define PC_KEYCODE_BREAK           126

#define PC_KEYCODE_DELETE          127
#define PC_KEYCODE_INSERT          128
#define PC_KEYCODE_HOME            129
#define PC_KEYCODE_END             130
#define PC_KEYCODE_PAGE_UP         131
#define PC_KEYCODE_PAGE_DOWN       132

#define PC_KEYCODE_NUMPAD_ASTERISK 142
#define PC_KEYCODE_NUMPAD_PLUS     143
#define PC_KEYCODE_NUMPAD_MINUS    145
#define PC_KEYCODE_NUMPAD_PERIOD   146
#define PC_KEYCODE_NUMPAD_SLASH    147
#define PC_KEYCODE_NUMPAD_0        148
#define PC_KEYCODE_NUMPAD_1        149
#define PC_KEYCODE_NUMPAD_2        150
#define PC_KEYCODE_NUMPAD_3        151
#define PC_KEYCODE_NUMPAD_4        152
#define PC_KEYCODE_NUMPAD_5        153
#define PC_KEYCODE_NUMPAD_6        154
#define PC_KEYCODE_NUMPAD_7        155
#define PC_KEYCODE_NUMPAD_8        156
#define PC_KEYCODE_NUMPAD_9        157
#define PC_KEYCODE_NUMPAD_NUM_LOCK 158
#define PC_KEYCODE_NUMPAD_EQUALS   161

#define PC_KEYCODE_F1              201
#define PC_KEYCODE_F2              202
#define PC_KEYCODE_F3              203
#define PC_KEYCODE_F4              204
#define PC_KEYCODE_F5              205
#define PC_KEYCODE_F6              206
#define PC_KEYCODE_F7              207
#define PC_KEYCODE_F8              208
#define PC_KEYCODE_F9              209
#define PC_KEYCODE_F10             210
#define PC_KEYCODE_F11             211
#define PC_KEYCODE_F12             212
#define PC_KEYCODE_F13             213
#define PC_KEYCODE_F14             214
#define PC_KEYCODE_F15             215
#define PC_KEYCODE_F16             216
#define PC_KEYCODE_F17             217
#define PC_KEYCODE_F18             218
#define PC_KEYCODE_F19             219

static int Plasmacore_syscode_to_keycode_map[128] =
{
  PC_KEYCODE_A,               //   0 -> a
  PC_KEYCODE_S,               //   1 -> s
  PC_KEYCODE_D,               //   2 -> d
  PC_KEYCODE_F,               //   3 -> f
  PC_KEYCODE_H,               //   4 -> h
  PC_KEYCODE_G,               //   5 -> g
  PC_KEYCODE_Z,               //   6 -> z
  PC_KEYCODE_X,               //   7 -> x
  PC_KEYCODE_C,               //   8 -> c
  PC_KEYCODE_V,               //   9 -> v
  0,                          //  10 -> (unknown)
  PC_KEYCODE_B,               //  11 -> b
  PC_KEYCODE_Q,               //  12 -> q
  PC_KEYCODE_W,               //  13 -> w
  PC_KEYCODE_E,               //  14 -> e
  PC_KEYCODE_R,               //  15 -> r
  PC_KEYCODE_Y,               //  16 -> y
  PC_KEYCODE_T,               //  17 -> t
  PC_KEYCODE_1,               //  18 -> 1
  PC_KEYCODE_2,               //  19 -> 2
  PC_KEYCODE_3,               //  20 -> 3
  PC_KEYCODE_4,               //  21 -> 4
  PC_KEYCODE_6,               //  22 -> 6
  PC_KEYCODE_5,               //  23 -> 5
  PC_KEYCODE_EQUALS,          //  24 -> EQUALS
  PC_KEYCODE_9,               //  25 -> 9
  PC_KEYCODE_7,               //  26 -> 7
  PC_KEYCODE_MINUS,           //  27 -> MINUS
  PC_KEYCODE_8,               //  28 -> 8
  PC_KEYCODE_0,               //  29 -> 0
  PC_KEYCODE_CLOSE_BRACKET,   //  30 -> ]
  PC_KEYCODE_O,               //  31 -> o
  PC_KEYCODE_U,               //  32 -> u
  PC_KEYCODE_OPEN_BRACKET,    //  33 -> [
  PC_KEYCODE_I,               //  34 -> i
  PC_KEYCODE_P,               //  35 -> p
  PC_KEYCODE_ENTER,           //  36 -> ENTER
  PC_KEYCODE_L,               //  37 -> l
  PC_KEYCODE_J,               //  38 -> j
  PC_KEYCODE_APOSTROPHE,      //  39 -> '
  PC_KEYCODE_K,               //  40 -> k
  PC_KEYCODE_SEMICOLON,       //  41 -> ;
  PC_KEYCODE_BACKSLASH,       //  42 -> '\'
  PC_KEYCODE_COMMA,           //  43 -> ,
  PC_KEYCODE_SLASH,           //  44 -> /
  PC_KEYCODE_N,               //  45 -> n
  PC_KEYCODE_M,               //  46 -> m
  PC_KEYCODE_PERIOD,          //  47 -> .
  PC_KEYCODE_TAB,             //  48 -> TAB
  PC_KEYCODE_SPACE,           //  49 -> SPACE
  PC_KEYCODE_BACKQUOTE,       //  50 -> grave (back quote)
  PC_KEYCODE_BACKSPACE,       //  51 -> BACKSPACE
  0,                          //  52 -> (unknown)
  PC_KEYCODE_ESCAPE,          //  53 -> ESC
  0,                          //  54 -> (unknown)
  0,                          //  55 -> (unknown)
  0,                          //  56 -> (unknown)
  0,                          //  57 -> (unknown)
  0,                          //  58 -> (unknown)
  0,                          //  59 -> (unknown)
  0,                          //  60 -> (unknown)
  0,                          //  61 -> (unknown)
  0,                          //  62 -> (unknown)
  0,                          //  63 -> (unknown)
  PC_KEYCODE_F17,             //  64 -> F17
  PC_KEYCODE_NUMPAD_PERIOD,   //  65 -> NUMPAD .
  0,                          //  66 -> (unknown)
  PC_KEYCODE_NUMPAD_ASTERISK, //  67 -> NUMPAD *
  0,                          //  68 -> (unknown)
  PC_KEYCODE_NUMPAD_PLUS,     //  69 -> NUMPAD +
  0,                          //  70 -> (unknown)
  PC_KEYCODE_NUMPAD_NUM_LOCK, //  71 -> CLEAR (Mac)
  0,                          //  72 -> (unknown)
  0,                          //  73 -> (unknown)
  0,                          //  74 -> (unknown)
  PC_KEYCODE_SLASH,           //  75 -> NUMPAD /
  0,                          //  76 -> (unknown)
  0,                          //  77 -> (unknown)
  PC_KEYCODE_NUMPAD_MINUS,    //  78 -> NUMPAD -
  PC_KEYCODE_F18,             //  79 -> F18
  PC_KEYCODE_F19,             //  80 -> F19
  PC_KEYCODE_NUMPAD_EQUALS,   //  81 -> NUMPAD =
  PC_KEYCODE_NUMPAD_0,        //  82 -> NUMPAD 0
  PC_KEYCODE_NUMPAD_1,        //  83 -> NUMPAD 1
  PC_KEYCODE_NUMPAD_2,        //  84 -> NUMPAD 2
  PC_KEYCODE_NUMPAD_3,        //  85 -> NUMPAD 3
  PC_KEYCODE_NUMPAD_4,        //  86 -> NUMPAD 4
  PC_KEYCODE_NUMPAD_5,        //  87 -> NUMPAD 5
  PC_KEYCODE_NUMPAD_6,        //  88 -> NUMPAD 6
  PC_KEYCODE_NUMPAD_7,        //  89 -> NUMPAD 7
  0,                          //  90 -> (unknown)
  PC_KEYCODE_NUMPAD_8,        //  91 -> NUMPAD 8
  PC_KEYCODE_NUMPAD_9,        //  92 -> NUMPAD 9
  0,                          //  93 -> (unknown)
  0,                          //  94 -> (unknown)
  0,                          //  95 -> (unknown)
  PC_KEYCODE_F5,              //  96 -> F5
  PC_KEYCODE_F6,              //  97 -> F6
  PC_KEYCODE_F7,              //  98 -> F7
  PC_KEYCODE_F3,              //  99 -> F3
  PC_KEYCODE_F8,              // 100 -> F8
  PC_KEYCODE_F9,              // 101 -> F9        
  0,                          // 102 -> (unknown)
  PC_KEYCODE_F11,             // 103 -> F11           
  0,                          // 104 -> (unknown)
  PC_KEYCODE_F13,             // 105 -> F13
  PC_KEYCODE_F16,             // 106 -> F16
  PC_KEYCODE_F14,             // 107 -> F14
  0,                          // 108 -> (unknown)
  PC_KEYCODE_F10,             // 109 -> F10
  0,                          // 110 -> (unknown)
  PC_KEYCODE_F12,             // 111 -> F12
  0,                          // 112 -> (unknown)
  PC_KEYCODE_F15,             // 113 -> F15
  0,                          // 114 -> (unknown)
  PC_KEYCODE_HOME,            // 115 -> HOME
  PC_KEYCODE_PAGE_UP,         // 116 -> PAGE UP
  PC_KEYCODE_DELETE,          // 117 -> DELETE
  PC_KEYCODE_F4,              // 118 -> F4
  PC_KEYCODE_END,             // 119 -> END
  PC_KEYCODE_F2,              // 120 -> F2
  PC_KEYCODE_PAGE_DOWN,       // 121 -> PAGE DOWN
  PC_KEYCODE_F1,              // 122 -> F1
  PC_KEYCODE_LEFT_ARROW,      // 123 -> LEFT ARROW
  PC_KEYCODE_RIGHT_ARROW,     // 124 -> RIGHT ARROW
  PC_KEYCODE_DOWN_ARROW,      // 125 -> DOWN ARROW
  PC_KEYCODE_UP_ARROW,        // 126 -> UP ARROW
  0                           // 127 -> (unknown)
};

@interface PlasmacoreView()
{
  int               window_id;
  SuperCPP::CString view_name;
  int               key_modifier_flags;
}
@end

@implementation PlasmacoreView
- (void)handleDisplayLinkUpdate
{
  @synchronized ([Plasmacore singleton])
  {
    [super handleDisplayLinkUpdate];
  }
}

- (BOOL)becomeFirstResponder
{
  if ( ![super becomeFirstResponder] ) return NO;
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.focus_gained"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m send];
  return YES;
}

- (NSPoint)convertEventPositionToLocalCoordinates:(NSEvent*)event
{
  NSPoint point = [event locationInWindow];
  return [self convertPoint:point fromView:nil];
}

- (void) handleModifiedKey:(int)modified withMask:(int)mask pcKeycode:(int)keycode
{
  if ((modified & mask) == mask)
  {
    PlasmacoreMessage* m;
    bool pressed = !(key_modifier_flags & mask);
    if (pressed) m = [PlasmacoreMessage messageWithType:"View.key_press"];
    else         m = [PlasmacoreMessage messageWithType:"View.key_release"];
    [m setInt32:"window_id" value:window_id];
    [m setCString:"view_name" value:view_name];
    [m setInt32:"keycode" value:keycode];
    [m send];
  }
}

- (void) flagsChanged:(NSEvent*)event
{
  int new_flags = (int) [event modifierFlags];
  int modified = key_modifier_flags ^ new_flags;

  [self handleModifiedKey:modified withMask:NSAlphaShiftKeyMask pcKeycode:PC_KEYCODE_CAPS_LOCK];

  [self handleModifiedKey:modified withMask:(NSShiftKeyMask|0x2) pcKeycode:PC_KEYCODE_LEFT_SHIFT];
  [self handleModifiedKey:modified withMask:(NSShiftKeyMask|0x4) pcKeycode:PC_KEYCODE_RIGHT_SHIFT];

  [self handleModifiedKey:modified withMask:(NSControlKeyMask|0x1)    pcKeycode:PC_KEYCODE_LEFT_CONTROL];
  [self handleModifiedKey:modified withMask:(NSControlKeyMask|0x2000) pcKeycode:PC_KEYCODE_RIGHT_CONTROL];

  [self handleModifiedKey:modified withMask:(NSAlternateKeyMask|0x20) pcKeycode:PC_KEYCODE_LEFT_ALT];
  [self handleModifiedKey:modified withMask:(NSAlternateKeyMask|0x40) pcKeycode:PC_KEYCODE_RIGHT_ALT];

  // Command key
  [self handleModifiedKey:modified withMask:(NSCommandKeyMask|0x08) pcKeycode:PC_KEYCODE_LEFT_OS];
  [self handleModifiedKey:modified withMask:(NSCommandKeyMask|0x10) pcKeycode:PC_KEYCODE_RIGHT_OS];

  [self handleModifiedKey:modified withMask:NSNumericPadKeyMask pcKeycode:PC_KEYCODE_NUMPAD_ENTER];

  key_modifier_flags = new_flags;
}

- (void) keyDown:(NSEvent*)event
{
  int syscode = [event keyCode] & 0x7f;
  int keycode = Plasmacore_syscode_to_keycode_map[ syscode ];
  NSString* characters = [event characters];
  for (int i=0; i<characters.length; ++i)
  {
    int unicode = [characters characterAtIndex:i];
    if (unicode >= 0xD800 && unicode <= 0xDBFF && i+1<characters.length)
    {
      // Transform surrogate pair into extended Unicode
      ++i;
      unicode = 0x10000 + (((unicode - 0xd800)<<10) | ([characters characterAtIndex:i]-0xdc00));
    }

    PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.key_press"];
    [m setInt32:"window_id" value:window_id];
    [m setCString:"view_name" value:view_name];
    [m setInt32:"unicode" value:unicode];
    [m setInt32:"keycode" value:keycode];
    [m setInt32:"syscode" value:syscode];
    [m setLogical:"is_repeat" value:[event isARepeat]];
    [m send];
  }
}

- (void) keyUp:(NSEvent*)event
{
  int syscode = [event keyCode] & 0x7f;
  int keycode = Plasmacore_syscode_to_keycode_map[ syscode ];
  NSString* characters = [event characters];
  for (int i=0; i<characters.length; ++i)
  {
    int unicode = [characters characterAtIndex:i];
    if (unicode >= 0xD800 && unicode <= 0xDBFF && i+1<characters.length)
    {
      // Transform surrogate pair into extended Unicode
      ++i;
      unicode = 0x10000 + (((unicode - 0xd800)<<10) | ([characters characterAtIndex:i]-0xdc00));
    }

    PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.key_release"];
    [m setInt32:"window_id" value:window_id];
    [m setCString:"view_name" value:view_name];
    [m setInt32:"unicode" value:unicode];
    [m setInt32:"keycode" value:keycode];
    [m setInt32:"syscode" value:syscode];
    [m send];
  }
}

- (void) mouseDown:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_button_press"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m setInt32:"button" value:0];
  [m send];
}

- (void) mouseDragged:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_move"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m send];
}

- (void) mouseMoved:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_move"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m send];
}

- (void) mouseUp:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_button_release"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m setInt32:"button" value:0];
  [m send];
}

- (void)onCreate
{
  window_id = [[Plasmacore singleton] idOfResource:self.window.windowController];
  view_name = [[self valueForKey:@"name"] UTF8String];
  NSLog( @"PlasmacoreView '%@' created in Window %d\n", [self valueForKey:@"name"], window_id );

  [self.window setAcceptsMouseMovedEvents:YES];
  [[self window] makeFirstResponder:self];
}

- (void)onDraw
{
  PlasmacoreMessage* m_draw = [PlasmacoreMessage messageWithType:"View.draw"];
  [m_draw setInt32:"window_id" value:window_id];
  [m_draw setCString:"view_name" value:view_name];
  [m_draw setInt32:"width"  value:(int)[self frame].size.width];
  [m_draw setInt32:"height" value:(int)[self frame].size.height];
  [m_draw push];
}

- (void)onStart
{
}

- (void)onStop
{
}

- (void)onUpdate
{
  PlasmacoreMessage* m_draw = [PlasmacoreMessage messageWithType:"View.update"];
  [m_draw setInt32:"window_id" value:window_id];
  [m_draw setCString:"view_name" value:view_name];
  [m_draw setInt32:"width"  value:(int)[self frame].size.width];
  [m_draw setInt32:"height" value:(int)[self frame].size.height];
  [m_draw push];
}

- (BOOL)resignFirstResponder
{
  if ( ![super resignFirstResponder] ) return NO;
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.focus_lost"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m send];
  return YES;
}

- (void) rightMouseDown:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_button_press"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m setInt32:"button" value:1];
  [m send];
}

- (void) rightMouseDragged:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_move"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m send];
}

- (void) rightMouseUp:(NSEvent*)event
{
  NSPoint point = [self convertEventPositionToLocalCoordinates:event];
  PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_button_release"];
  [m setInt32:"window_id" value:window_id];
  [m setCString:"view_name" value:view_name];
  [m setReal64:"x" value:point.x];
  [m setReal64:"y" value:self.bounds.size.height - point.y];
  [m setInt32:"button" value:1];
  [m send];
}

- (void) scrollWheel:(NSEvent*)event
{
  double dx = [event deltaX];
  double dy = [event deltaY];

  if (dx >= 0.0001 || dx <= -0.0001 || dy >= 0.0001 || dy <= -0.0001)
  {
    PlasmacoreMessage* m = [PlasmacoreMessage messageWithType:"View.mouse_scroll"];
    [m setInt32:"window_id" value:window_id];
    [m setCString:"view_name" value:view_name];
    [m setReal64:"dx" value:dx];
    [m setReal64:"dy" value:dy];
    [m send];
  }
}
@end


