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

  func getString( name:String, default_value:String="" )->String
  {
    var result = default_value
    if let offset = entries[ name ]
    {
      position = offset
      switch (offset)
      {
        case DataType.STRING:
          result = readString()
        default:
          break
      }
      position = data.count
    }
    return result
  }

  func readByte()->Int
  {
    return (position < data.count) ?  Int(data[position++]) : 0
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

  func setString( name:String, value:String )->PlasmacoreMessage
  {
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.STRING )
    writeString( value )
    return self
  }

  func setInt32( name:String, value:Int )->PlasmacoreMessage
  {
    writeString( name )
    entries[ name ] = position
    writeIntX( DataType.INT32 )
    writeIntX( value )
    return self
  }

  private func writeByte( value:Int )
  {
    if (position == data.count)
    {
      data.append( UInt8(value) )
    }
    else
    {
      data[ position++ ] = UInt8( value )
    }
  }

  private func writeInt32( value:Int )
  {
    writeByte( value >> 24 )
    writeByte( value >> 16 )
    writeByte( value >> 8  )
    writeByte( value )
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

  private func writeString( value:String )
  {
    writeIntX( value.utf16.count )
    for ch in value.utf16
    {
      writeIntX( Int(ch) )
    }
  }
}

