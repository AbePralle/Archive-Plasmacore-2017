#if os(OSX)
  import Cocoa
#endif

class PlasmacoreMessage
{
  struct DataType
  {
    static let STRING  = 1
    static let REAL64  = 2
    static let INT64   = 3
    static let INT32   = 4
    static let LOGICAL = 5
    static let BYTES   = 6
  }

  static var next_message_id = 1

  var type       : String
  var message_id : Int

  var data          = [UInt8]()
  var string_buffer = [Character]()
  var entries  = [String:Int]()
  var position = 0

  convenience init( type:String )
  {
    self.init( type:type, message_id:PlasmacoreMessage.next_message_id )
    PlasmacoreMessage.next_message_id += 1
  }

  convenience init( data:[UInt8] )
  {
    self.init()
    self.data = data
    type = readString()
    message_id = readIntX()
    while (indexAnother()) {}
  }

  convenience init( type:String, message_id:Int )
  {
    self.init()
    self.type = type
    self.message_id = message_id
    writeString( type )
    writeIntX( message_id )
  }

  init()
  {
    type = "Unspecified"
    message_id = 0
  }

  func createReply()->PlasmacoreMessage
  {
    return PlasmacoreMessage( type:"<reply>", message_id:message_id )
  }

  func getBytes( name:String )->[UInt8]
  {
    if let offset = entries[ name ]
    {
      position = offset
      switch (readIntX())
      {
      case DataType.BYTES:
        let count = readIntX()
        var buffer = [UInt8]()
        buffer.reserveCapacity( count )
        for _ in 1...count
        {
          buffer.append( UInt8(readByte()) )
        }
        return buffer
      default:
        break
      }
    }
    return [UInt8]()
  }

  func getDictionary( name:String, default_value:String="" )->[String:AnyObject]?
  {
    let jsonString = getString(name: name, default_value: default_value)
    if let data = jsonString.data(using: String.Encoding.utf8) {
        do {
            return try JSONSerialization.jsonObject(with: data, options: []) as? [String:AnyObject]
        } catch let error as NSError {
            print(error)
        }
    }
    return nil

  }

  func getInt32( name:String, default_value:Int=0 )->Int
  {
    if let offset = entries[ name ]
    {
      position = offset
      switch (readIntX())
      {
      case DataType.REAL64:
        return Int(readReal64())

      case DataType.INT64:
        return Int(readInt64X())

      case DataType.INT32:
        return readIntX()

      case DataType.LOGICAL:
        return readIntX()

      default:
        return default_value
      }
    }
    return default_value

  }

  func getInt64( name:String, default_value:Int64=0 )->Int64
  {
    if let offset = entries[ name ]
    {
      position = offset
      switch (readIntX())
      {
      case DataType.REAL64:
        return Int64(readReal64())

      case DataType.INT64:
        return readInt64X()

      case DataType.INT32:
        return Int64(readIntX())

      case DataType.LOGICAL:
        return Int64(readIntX())

      default:
        return default_value
      }
    }
    return default_value

  }

  func getLogical( name:String, default_value:Bool=false )->Bool
  {
    return getInt32( name: name, default_value:(default_value ? 1 : 0) ) != 0
  }

  func getReal64( name:String, default_value:Double=0.0 )->Double
  {
    if let offset = entries[ name ]
    {
      position = offset
      switch (readIntX())
      {
      case DataType.REAL64:
        return readReal64()

      case DataType.INT64:
        return Double(readInt64X())

      case DataType.INT32:
        return Double(readIntX())

      case DataType.LOGICAL:
        return Double(readIntX())

      default:
        return default_value
      }
    }
    return default_value

  }

  func getString( name:String, default_value:String="" )->String
  {
    if let offset = entries[ name ]
    {
      position = offset
      switch (readIntX())
      {
        case DataType.STRING:
          return readString()
        default:
          return default_value
      }
    }
    return default_value
  }

  func indexAnother()->Bool
  {
    if (position == data.count) { return false }
    let name = readString()
    entries[ name ] = position

    switch (readIntX())
    {
    case DataType.STRING:
      let count = readIntX()
      for _ in 1...count
      {
        _ = readIntX()
      }
      return true

    case DataType.REAL64:
      _ = readReal64()
      return true

    case DataType.INT64:
      _ = readInt64X()
      return true

    case DataType.INT32:
      _ = readIntX()
      return true

    case DataType.LOGICAL:
      _ = readIntX()
      return true

    case DataType.BYTES:
      position += readIntX()
      return true

    default:
      return false
    }
  }

  func send()
  {
    Plasmacore.singleton.send( self );
  }

  func send_rsvp( _ callback:@escaping ((PlasmacoreMessage)->Void) )
  {
    Plasmacore.singleton.send_rsvp( self, callback:callback );
  }

  @discardableResult
  func set( name:String, value:[UInt8] )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.BYTES )
    writeIntX( value.count )
    for byte in value
    {
      writeByte( Int(byte) )
    }
    return self
  }

  @discardableResult
  func set( name:String, value:Int64 )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.INT64 )
    writeIntX( Int(value>>32) )
    writeIntX( Int(value) )
    return self
  }

  @discardableResult
  func set( name:String, value:Int )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.INT32 )
    writeIntX( value )
    return self
  }

  @discardableResult
  func set( name:String, value:Bool )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.LOGICAL )
    writeIntX( value ? 1 : 0 )
    return self
  }

  @discardableResult
  func set( name:String, value:Double )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.REAL64 )
    writeReal64( value )
    return self
  }

  @discardableResult
  func set( name:String, value:String )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.STRING )
    writeString( value )
    return self
  }

  //---------------------------------------------------------------------------
  // PRIVATE
  //---------------------------------------------------------------------------

  fileprivate func readByte()->Int
  {
    if (position >= data.count) { return 0 }
    position += 1
    return Int(data[position-1])
  }

  fileprivate func readInt64X()->Int64
  {
    let result = Int64( readIntX() ) << 32
    return result | Int64( UInt32(readIntX()) )
  }

  fileprivate func readInt32()->Int
  {
    var result = readByte()
    result = (result << 8) | readByte()
    result = (result << 8) | readByte()
    return (result << 8) | readByte()
  }

  fileprivate func readIntX()->Int
  {
    let b = readByte()
    if ((b & 0xc0) != 0x80)
    {
      return b
    }

    switch (b & 0x30)
    {
      case 0x00:
        return ((b & 15) << 8) | readByte()

      case 0x10:
        position += 2
        if (position > data.count)
        {
          position = data.count
          return 0
        }
        return ((b & 15) << 16) | (Int(data[position-2])<<8) | Int(data[position-1])

      case 0x20:
        position += 3
        if (position > data.count)
        {
          position = data.count
          return 0
        }
        let b1 = ((b & 15) << 24)
        let b2 = (Int(data[position-3])<<16)
        let b3 = (Int(data[position-2]<<8))
        let b4 = Int(data[position-1])
        return b1 | b2 | b3 | b4

      default:
        return readInt32()
    }
  }

  fileprivate func readReal64()->Double
  {
    var n = UInt64( readInt32() ) << 32
    n = n | UInt64( UInt32(readInt32()) )
    return Double(bitPattern:n)
  }

  fileprivate func readString()->String
  {
    string_buffer.removeAll( keepingCapacity:true )
    let count = readIntX()
    for _ in 0..<count
    {
      string_buffer.append( Character(UnicodeScalar(readIntX())!) )
    }
    return String( string_buffer )
  }

  fileprivate func writeByte( _ value:Int )
  {
    position += 1
    if (position > data.count)
    {
      data.append( UInt8(value&255) )
    }
    else
    {
      data[ position-1 ] = UInt8( value )
    }
  }

  fileprivate func writeInt32( _ value:Int )
  {
    writeByte( value >> 24 )
    writeByte( value >> 16 )
    writeByte( value >> 8  )
    writeByte( value )
  }

  fileprivate func writeInt64X( _ value:Int64 )
  {
    writeIntX( Int(value>>32) )
    writeIntX( Int(value) )
  }

  fileprivate func writeIntX( _ value:Int )
  {
    if (value >= -64 && value <= 127)
    {
      writeByte( value )
    }
    else if (value >= -0x400 && value <= 0x3ff)
    {
      writeByte( 0x80 | ((value >> 8) & 15) )
      writeByte( value )
    }
    else if (value >= -0x40000 && value <= 0x3ffff)
    {
      writeByte( 0x90 | ((value >> 16) & 15) )
      writeByte( value >> 8 )
      writeByte( value )
    }
    else if (value >= -0x4000000 && value <= 0x3ffffff)
    {
      writeByte( 0xa0 | ((value >> 24) & 15) )
      writeByte( value >> 16 )
      writeByte( value >> 8 )
      writeByte( value )
    }
    else
    {
      writeByte( 0xb0 )
      writeInt32( value )
    }
  }

  fileprivate func writeReal64( _ value:Double )
  {
    let bits = value.bitPattern
    writeInt32( Int((bits>>32)&0xFFFFffff) )
    writeInt32( Int(bits&0xFFFFffff) )
  }

  fileprivate func writeString( _ value:String )
  {
    writeIntX( value.unicodeScalars.count )
    for ch in value.unicodeScalars
    {
      writeIntX( Int(ch.value) )
    }
  }
}

