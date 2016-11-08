#if os(OSX)
import Cocoa
#else
import Foundation
#endif

class Plasmacore
{
  static let _singleton = Plasmacore()

  static var singleton:Plasmacore
  {
    get
    {
      let result = _singleton
      if (result.is_launched) { return result }
      return result.configure().launch()
    }
  }

  class func onLaunch()
  {
    singleton.start()   // Sends Application.launch message automatically on first start
  }

  class func onStop()
  {
    singleton.stop()
    PlasmacoreMessage( type:"Application.stop" ).send()
  }

  class func onStart()
  {
    PlasmacoreMessage( type:"Application.start" ).send()
    singleton.start()
  }

  class func onSave()
  {
    singleton.stop()
    PlasmacoreMessage( type:"Application.save" ).send()
  }

  var is_configured = false
  var is_launched   = false

  var nextHandlerID = 1
  var idleUpdateFrequency = 0.5

  var pending_message_data = [UInt8]()
  var io_buffer = [UInt8]()

  var is_sending = false
  var update_requested = false

  var handlers = [String:[PlasmacoreMessageHandler]]()
  var handlers_by_id = [Int:PlasmacoreMessageHandler]()
  var reply_handlers = [Int:PlasmacoreMessageHandler]()
  var resources = [Int:AnyObject]()

  var update_timer : Timer?

  fileprivate init()
  {
  }

  @discardableResult
  func addMessageHandler( type:String, handler:@escaping ((PlasmacoreMessage)->Void) )->Int
  {
    objc_sync_enter( self ); defer { objc_sync_exit(self) }   // @synchronized (self)

    let info = PlasmacoreMessageHandler( handlerID:nextHandlerID, type:type, callback:handler )
    nextHandlerID += 1

    handlers_by_id[ info.handlerID ] = info
    if handlers[ type ] != nil
    {
      handlers[ type ]!.append( info )
    }
    else
    {
      var list = [PlasmacoreMessageHandler]()
      list.append( info )
      handlers[ type ] = list
    }
    return info.handlerID
  }

  @discardableResult
  func configure()->Plasmacore
  {
    if (is_configured) { return self }
    is_configured = true

    addMessageHandler( type: "<reply>", handler:
      {
        (m:PlasmacoreMessage) in
          if let info = Plasmacore.singleton.reply_handlers.removeValue( forKey: m.message_id )
          {
            info.callback( m )
          }
      }
    )

    #if os(OSX)
    addMessageHandler( "Window.create", handler:
      {
        (m:PlasmacoreMessage) in
        let name = m.getString( "name" )
        var className = name
        if let bundleID = NSBundle.mainBundle().bundleIdentifier
        {
          if let dotIndex = Plasmacore.lastIndexOf( bundleID, lookFor:"." )
          {
            className = "\(bundleID.substringFromIndex(bundleID.startIndex.advancedBy(dotIndex+1))).\(name)"
          }
        }

        let controller : NSWindowController
        if let controllerType = NSClassFromString( className ) as? NSWindowController.Type
        {
          NSLog( "Found controller \(className)" )
          controller = controllerType.init( windowNibName:name )
        }
        else if let controllerType = NSClassFromString( name ) as? NSWindowController.Type
        {
          NSLog( "Found controller \(name)" )
          controller = controllerType.init( windowNibName:name )
        }
        else
        {
          NSLog( "===============================================================================" )
          NSLog( "ERROR" )
          NSLog( "  No class found named \(name) or \(className)." )
          NSLog( "===============================================================================" )
          controller = NSWindowController( windowNibName:name )
        }

        Plasmacore.singleton.resources[ m.getInt32("id") ] = controller
        NSLog( "Controller window:\(controller.window)" )
      }
    )

    addMessageHandler( "Window.show", handler:
      {
        (m:PlasmacoreMessage) in
          let window_id = m.getInt32( "id" )
          if let window = Plasmacore.singleton.resources[ window_id ] as? NSWindowController
          {
            window.showWindow( self )
          }
      }
    )
    #endif // os(OSX)

    RogueInterface_set_arg_count( Int32(CommandLine.arguments.count) )
    for (index,arg) in CommandLine.arguments.enumerated()
    {
      RogueInterface_set_arg_value( Int32(index), arg )
    }

    RogueInterface_configure();
    return self
  }

  func getResourceID( _ resource:AnyObject? )->Int
  {
    guard let resource = resource else { return 0 }

    for (key,value) in resources
    {
      if (value === resource) { return key }
    }
    return 0
  }

  @discardableResult
  func launch()->Plasmacore
  {
    if (is_launched) { return self }
    is_launched = true

    RogueInterface_launch()
    var m = PlasmacoreMessage( type:"Application.launch" )
#if os(OSX)
    m.set( "is_window_based", value:true )
#endif
    m.send()
    return self
  }

  static func lastIndexOf( _ st:String, lookFor:String )->Int?
  {
    if let r = st.range( of: lookFor, options:.backwards )
    {
      return st.characters.distance(from: st.startIndex, to: r.lowerBound)
    }

    return nil
  }

  func relaunch()->Plasmacore
  {
    PlasmacoreMessage( type:"Application.launch" ).set( name:"is_window_based", value:true ).send()
    return self
  }

  func removeMessageHandler( _ handlerID:Int )
  {
    objc_sync_enter( self ); defer { objc_sync_exit(self) }   // @synchronized (self)

    if let handler = handlers_by_id[ handlerID ]
    {
      handlers_by_id.removeValue( forKey: handlerID )
      if let handler_list = handlers[ handler.type ]
      {
        for i in 0..<handler_list.count
        {
          if (handler_list[i] === handler)
          {
            handlers[ handler.type ]!.remove( at: i );
            return;
          }
        }
      }
    }
  }

  func send( _ m:PlasmacoreMessage )
  {
    objc_sync_enter( self ); defer { objc_sync_exit(self) }    // @synchronized (self)

    let size = m.data.count
    pending_message_data.append( UInt8((size>>24)&255) )
    pending_message_data.append( UInt8((size>>16)&255) )
    pending_message_data.append( UInt8((size>>8)&255) )
    pending_message_data.append( UInt8(size&255) )
    for b in m.data
    {
      pending_message_data.append( b )
    }
    update()
  }

  func send_rsvp( _ m:PlasmacoreMessage, callback:@escaping ((PlasmacoreMessage)->Void) )
  {
    objc_sync_enter( self ); defer { objc_sync_exit(self) }   // @synchronized (self)

    reply_handlers[ m.message_id ] = PlasmacoreMessageHandler( handlerID:nextHandlerID, type:"<reply>", callback:callback )
    nextHandlerID += 1
    send( m )
  }

  func setIdleUpdateFrequency( _ f:Double )->Plasmacore
  {
    idleUpdateFrequency = f
    if (update_timer != nil)
    {
      stop()
      start()
    }
    return self
  }

  func start()
  {
    if ( !is_launched ) { configure().launch() }

    if (update_timer === nil)
    {
      update_timer = Timer.scheduledTimer( timeInterval: idleUpdateFrequency, target:self, selector: #selector(Plasmacore.update), userInfo:nil, repeats: true )
    }
    update()
  }

  func stop()
  {
    if (update_timer !== nil)
    {
      update_timer!.invalidate()
      update_timer = nil
    }
  }

  @objc func update()
  {
    objc_sync_enter( self ); defer { objc_sync_exit(self) }   // @synchronized (self)

    if (is_sending)
    {
      update_requested = true
      return
    }

    is_sending = true

    // Execute a small burst of message dispatching and receiving.  Stop after
    // 10 iterations or when there are no new messages.  Global state updates
    // are frequency capped to 1/60 second intervals and draws are synced to
    // the display refresh so this isn't triggering large amounts of extra work.
    for _ in 1...10
    {
      update_requested = false

      // Swap pending data with io_buffer data
      let temp = io_buffer
      io_buffer = pending_message_data
      pending_message_data = temp

      let received_data = RogueInterface_send_messages( io_buffer, Int32(io_buffer.count) )
      let count = received_data!.count
      received_data!.withUnsafeBytes{ ( bytes:UnsafePointer<UInt8>)->Void in
        //{(bytes: UnsafePointer<CChar>)->Void in
        //Use `bytes` inside this closure
        //...

      var read_pos = 0
      while (read_pos+4 <= count)
      {
        var size = Int( bytes[read_pos] ) << 24
        size |= Int( bytes[read_pos+1] ) << 16
        size |= Int( bytes[read_pos+2] ) << 8
        size |= Int( bytes[read_pos+3] )
        read_pos += 4;

        if (read_pos + size <= count)
        {
          var message_data = [UInt8]()
          message_data.reserveCapacity( size )
          for i in 0..<size
          {
            message_data.append( bytes[read_pos+i] )
          }

          let m = PlasmacoreMessage( data:message_data )
          //NSLog( "Received message type \(m.type)" )
          if let handler_list = handlers[ m.type ]
          {
            for handler in handler_list
            {
              handler.callback( m )
            }
          }
        }
        else
        {
          NSLog( "*** Skipping message due to invalid size." )
        }
        read_pos += size
      }
      }

      io_buffer.removeAll()

      if ( !update_requested )
      {
        break
      }
    }

    is_sending = false

    if (update_requested)
    {
      // There are still some pending messages after 10 iterations.  Schedule another round
      // in 1/60 second instead of the usual 1.0 seconds.
      Timer.scheduledTimer( timeInterval: 1.0/60.0, target:self, selector: #selector(Plasmacore.update), userInfo:nil, repeats:false )
    }
  }
}

class PlasmacoreMessageHandler
{
  var handlerID : Int
  var type      : String
  var callback  : ((PlasmacoreMessage)->Void)

  init( handlerID:Int, type:String, callback:@escaping ((PlasmacoreMessage)->Void) )
  {
    self.handlerID = handlerID
    self.type = type
    self.callback = callback
  }
}

