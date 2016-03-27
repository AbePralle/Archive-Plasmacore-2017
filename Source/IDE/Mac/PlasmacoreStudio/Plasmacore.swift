import Cocoa

class Plasmacore
{
  static let singleton = Plasmacore()

  var nextHandlerID = 1

  var pending_message_data = [UInt8]()
  var io_buffer = [UInt8]()

  var is_sending = false
  var update_requested = false

  var handlers = [String:[PlasmacoreMessageHandler]]()
  var handlers_by_id = [Int:PlasmacoreMessageHandler]()
  var reply_handlers = [Int:PlasmacoreMessageHandler]()
  var resources = [Int:AnyObject]()
  
  var update_timer : NSTimer?

  private init()
  {
  }

  func addMessageHandler( type:String, handler:((PlasmacoreMessage)->Void) )->Int
  {
    let info = PlasmacoreMessageHandler( handlerID:nextHandlerID++, type:type, callback:handler )
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

  func configure()->Plasmacore
  {
    addMessageHandler( "<reply>", handler:
      {
        (m:PlasmacoreMessage) in
          if let info = Plasmacore.singleton.reply_handlers.removeValueForKey( m.message_id )
          {
            info.callback( m )
          }
      }
    )

    addMessageHandler( "Window.create", handler:
      {
        (m:PlasmacoreMessage) in
          let reply = m.createReply()
          let name = m.getString( "name" )
          if let window = Plasmacore_create_window( name )
          {
            reply.setLogical( "success", value:true )
            Plasmacore.singleton.resources[ m.getInt32("id") ] = window
          }
          else
          {
            reply.setLogical( "success", value:false )
            NSLog( "Window.create failed - could not find a WindowController or XIB named \"\(name)\"." )
          }
          reply.send()
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

    RogueInterface_set_arg_count( Int32(Process.arguments.count) )
    for (index,arg) in Process.arguments.enumerate()
    {
      RogueInterface_set_arg_value( Int32(index), arg )
    }

    RogueInterface_configure();
    return self
  }

  func launch()->Plasmacore
  {
    RogueInterface_launch();
    return self
  }

  func removeMessageHandler( handlerID:Int )
  {
    if let handler = handlers_by_id[ handlerID ]
    {
      handlers_by_id.removeValueForKey( handlerID )
      if let handler_list = handlers[ handler.type ]
      {
        for i in 0..<handler_list.count
        {
          if (handler_list[i] === handler)
          {
            handlers[ handler.type ]!.removeAtIndex( i );
            return;
          }
        }
      }
    }
  }

  func send( m:PlasmacoreMessage )
  {
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

  func send_rsvp( m:PlasmacoreMessage, callback:((PlasmacoreMessage)->Void) )
  {
    reply_handlers[ m.message_id ] = PlasmacoreMessageHandler( handlerID:nextHandlerID++, type:"<reply>", callback:callback )
    send( m )
  }

  func start()
  {
    if (update_timer === nil)
    {
      update_timer = NSTimer.scheduledTimerWithTimeInterval( 1.0, target:self, selector: "update", userInfo:nil, repeats: true )
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
      let count = received_data.length
      let bytes = UnsafePointer<UInt8>( received_data.bytes )
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
          for (var i=0; i<size; ++i)
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
      NSTimer.scheduledTimerWithTimeInterval( 1.0/60.0, target:self, selector: "update", userInfo:nil, repeats:false )
    }
  }
}

class PlasmacoreMessageHandler
{
  var handlerID : Int
  var type      : String
  var callback  : ((PlasmacoreMessage)->Void)

  init( handlerID:Int, type:String, callback:((PlasmacoreMessage)->Void) )
  {
    self.handlerID = handlerID
    self.type = type
    self.callback = callback
  }
}

