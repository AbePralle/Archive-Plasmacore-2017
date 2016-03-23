import Cocoa

class Plasmacore
{
  static let singleton = Plasmacore()

  var pending_message_data = [UInt8]()
  var io_buffer = [UInt8]()
  var is_sending = false

  private init()
  {
  }

  func configure()->Plasmacore
  {
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
          NSLog( "Message type \(m.type)" )
          NSLog( "Airline: \(m.getString("airline"))" )
          NSLog( "pi: \(m.getReal64("pi"))" )
          //NSLog( "Message type \(m.type), id:\(m.message_id)" )
        }
        else
        {
          NSLog( "*** Skipping message due to invalid size." )
        }
        read_pos += size
      }


      io_buffer.removeAll()
      //for i in 0..<received_ns_data.length
      //{
        //io_buffer.append( received_ns_data.bytes[i] )
      //}

      //NSLog( "Received \(received_ns_data.count) in response!" )

      io_buffer.removeAll()
      is_sending = false
    }
  }
}

