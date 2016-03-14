import Cocoa;

class PlasmacoreMessage
{
  static var next_message_id = 1;

  var type       : String
  var message_id : Int

  var data = [UInt8]()
  var position = 0;

  convenience init( type:String )
  {
    self.init( type:type, message_id:PlasmacoreMessage.next_message_id++ );
  }

  init( type:String, message_id:Int )
  {
    self.type = type;
    self.message_id = message_id;
    writeString( type );
    writeIntX( message_id );
  }
  
  private func writeByte( value:Int )
  {
    if (position == data.count)
    {
      data.append( UInt8(value) );
    }
    else
    {
      data[ position++ ] = UInt8( value );
    }
  }

  private func writeInt32( value:Int )
  {
    writeByte( value >> 24 );
    writeByte( value >> 16 );
    writeByte( value >> 8  );
    writeByte( value );
  }

  private func writeIntX( value:Int )
  {
    if (value >= -64 && value <= 127)
    {
      writeByte( value );
    }
    else if (value >= -0x400 && value <= 0x3ff)
    {
      writeByte( 0x80 | ((value >> 8) & 15) );
      writeByte( value );
    }
    else if (value >= -0x40000 && value <= 0x3ffff)
    {
      writeByte( 0x90 | ((value >> 16) & 15) );
      writeByte( value >> 8 );
      writeByte( value );
    }
    else if (value >= -0x4000000 && value <= 0x3ffffff)
    {
      writeByte( 0xa0 | ((value >> 24) & 15) );
      writeByte( value >> 16 );
      writeByte( value >> 8 );
      writeByte( value );
    }
    else
    {
      writeByte( 0xb0 );
      writeInt32( value );
    }
  }

  private func writeString( value:String )
  {
    writeIntX( value.utf16.count );
    for ch in value.utf16
    {
      writeIntX( Int(ch) );
    }
  }
}

