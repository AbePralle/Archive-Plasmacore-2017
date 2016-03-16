import Cocoa

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
  var string_buffer = [UInt16]()
  var entries  = [String:Int]()
  var position = 0

  convenience init( type:String )
  {
    self.init( type:type, message_id:PlasmacoreMessage.next_message_id++ )
  }

  init( type:String, message_id:Int )
  {
    self.type = type
    self.message_id = message_id
    writeString( type )
    writeIntX( message_id )
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
    return getInt32( name, default_value:(default_value ? 1 : 0) ) != 0
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

  func setInt64( name:String, value:Int64 )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.INT64 )
    writeIntX( Int(value>>32) )
    writeIntX( Int(value) )
    return self
  }

  func setInt32( name:String, value:Int )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.INT32 )
    writeIntX( value )
    return self
  }

  func setLogical( name:String, value:Bool )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.LOGICAL )
    writeIntX( value ? 1 : 0 )
    return self
  }

  func setReal64( name:String, value:Double )->PlasmacoreMessage
  {
    position = data.count
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.REAL64 )
    writeReal64( value )
    return self
  }

  func setString( name:String, value:String )->PlasmacoreMessage
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

  func readByte()->Int
  {
    return (position < data.count) ?  Int(data[position++]) : 0
  }

  func readInt64X()->Int64
  {
    let result = Int64( readIntX() ) << 32
    return result | Int64( UInt32(readIntX()) )
  }

  func readInt32()->Int
  {
    var result = readByte()
    result = (result << 8) | readByte()
    result = (result << 8) | readByte()
    return (result << 8) | readByte()
  }

  func readIntX()->Int
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
        return ((b & 15) << 24) | (Int(data[position-3])<<16) | (Int(data[position-2]<<8)) | Int(data[position-1])

      default:
        return readInt32()
    }
  }

  func readReal64()->Double
  {
    var n = UInt64( readInt32() ) << 32
    n = n | UInt64( UInt32(readInt32()) )
    return Double._fromBitPattern( n )
  }

  func readString()->String
  {
    string_buffer.removeAll( keepCapacity:true )
    let count = readIntX()
    for _ in 0..<count
    {
      string_buffer.append( UInt16(readIntX()) )
    }
    return String( utf16CodeUnits:string_buffer, count:count )
  }

  private func writeByte( value:Int )
  {
    if (position++ == data.count)
    {
      data.append( UInt8(value) )
    }
    else
    {
      data[ position-1 ] = UInt8( value )
    }
  }

  private func writeInt32( value:Int )
  {
    writeByte( value >> 24 )
    writeByte( value >> 16 )
    writeByte( value >> 8  )
    writeByte( value )
  }

  private func writeInt64X( value:Int64 )
  {
    writeIntX( Int(value>>32) )
    writeIntX( Int(value) )
  }

  private func writeIntX( value:Int )
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

  private func writeReal64( value:Double )
  {
    let bits = Double._BitsType( value )
    writeInt32( Int(bits>>32) )
    writeInt32( Int(bits) )
  }

  private func writeString( value:String )
  {
    writeIntX( value.utf16.count )
    for ch in value.utf16
    {
      writeIntX( Int(ch) )
    }
  }
}

