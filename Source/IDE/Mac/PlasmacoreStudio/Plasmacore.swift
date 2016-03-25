import Cocoa

class Plasmacore
{
  static let singleton = Plasmacore()

  var        nextHandlerID = 1

  var pending_message_data = [UInt8]()
  var io_buffer = [UInt8]()
  var is_sending = false
  var handlers = [String:[PlasmacoreMessageHandler]]()
  var handlers_by_id = [Int:PlasmacoreMessageHandler]()
  var reply_handlers = [Int:PlasmacoreMessageHandler]()
  
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
    let handlerID = Plasmacore.singleton.nextHandlerID

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
          NSLog( "Window.create 1, handlerID:\(handlerID)" )
          Plasmacore.singleton.removeMessageHandler( handlerID )
          PlasmacoreMessage( type:"Marco" ).send_rsvp(
            {
              (m:PlasmacoreMessage) in
                NSLog( "Got reply \(m.getString("message"))" )
            }
          )
      }
    )

    addMessageHandler( "Window.create", handler:
      {
        (m:PlasmacoreMessage) in
          NSLog( "Window.create 2" )
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
      update_timer = NSTimer.scheduledTimerWithTimeInterval( 1.0, target:self, selector: "update", userInfo: nil, repeats: true )
    }
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
    if ( !is_sending )
    {
      // This flag will cause recursive sends to queue up.
      is_sending = true

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
      is_sending = false
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

