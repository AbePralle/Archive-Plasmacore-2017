package org.plasmacore;

public class Message
{
  // ENUMERATE
  final static int DATA_TYPE_STRING  = 1;
  final static int DATA_TYPE_REAL64  = 2;
  final static int DATA_TYPE_INT64   = 3;
  final static int DATA_TYPE_INT32   = 4;
  final static int DATA_TYPE_LOGICAL = 5;
  final static int DATA_TYPE_BYTES   = 6;


  // GLOBAL PROPERTIES
  static int next_message_id = 1;


  // PROPERTIES
  public String type;
  int    message_id;
  byte[] data;

  // Using arrays instead of tables to reduce dynamic allocation overhead
  String[] keys;
  int[]    offsets;

  // METHODS
  public Message( String type )
  {
    this( type, next_message_id++ );
  }

  public Message( String type, int message_id )
  {
    this.type = type;
    this.message_id = message_id;
    type = MessageManager.consolidated_type_name( new_type )
    sent = false
    writer.clear

    writer.write_string( type )
    writer.write_int32x( message_id )

  method init( source:DataReader )
    timestamp = System.time
    keys.clear
    offsets.clear
    local size = source.read_int32
    source.read( data.clear, size )
    reader.reset
    type = read_type_name
    message_id = reader.read_int32x
    while (index_another) noAction

  method contains( name:String )->Logical
    return keys.locate(name).exists

  method create_reply->Message
    return MessageManager.create_message( "<reply>", message_id )

  method get( name:String )->Value
    local i = keys.locate_last( name )
    if (not i.exists) return UndefinedValue

    reader.seek( offsets[i.value] )

    which (reader.read_int32x)
      case DATA_TYPE_STRING
        return Value( reader.read_string( MessageManager.string_buffer.clear )->String )

      case DATA_TYPE_BYTES
        local count = reader.read_int32x
        MessageManager.string_buffer.clear
        forEach (1..count) MessageManager.string_buffer.print( reader.read->Character )
        return Value( MessageManager.string_buffer->String )

      case DATA_TYPE_REAL64
        return Value( reader.read_real64 )

      case DATA_TYPE_INT64
        return Value( reader.read_int64x )

      case DATA_TYPE_INT32
        return Value( reader.read_int32x )

      case DATA_TYPE_LOGICAL
        return Value( reader.read_int32x? )

      others
        return UndefinedValue
    endWhich

  method string( name:String, default="":String )->String
    local i = keys.locate_last( name )
    if (not i.exists) return default

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_STRING
        return reader.read_string( MessageManager.string_buffer.clear )->String
      case DATA_TYPE_BYTES
        local count = reader.read_int32x
        MessageManager.string_buffer.clear
        forEach (1..count) MessageManager.string_buffer.print( reader.read->Character )
        return MessageManager.string_buffer->String
    endWhich

    return default

  method string( name:String, buffer:StringBuilder )->StringBuilder
    local i = keys.locate_last( name )
    if (not i.exists) return buffer

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_STRING
        return reader.read_string( buffer )
      case DATA_TYPE_BYTES
        local count = reader.read_int32x
        buffer.reserve( count )
        forEach (1..count) buffer.print( reader.read->Character )
        return buffer
    endWhich

    return buffer

  method real64( name:String, default=0:Real64 )->Real64
    local i = keys.locate_last( name )
    if (not i.exists) return default

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_REAL64
        return reader.read_real64

      case DATA_TYPE_INT64
        return reader.read_int64x

      case DATA_TYPE_INT32, DATA_TYPE_LOGICAL
        return reader.read_int32x
    endWhich

    return default

  method int64( name:String, default=0:Int64 )->Int64
    local i = keys.locate_last( name )
    if (not i.exists) return default

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_REAL64
        return reader.read_real64->Int64

      case DATA_TYPE_INT64
        return reader.read_int64x

      case DATA_TYPE_INT32, DATA_TYPE_LOGICAL
        return reader.read_int32x
    endWhich

    return default

  method int32( name:String, default=0:Int32 )->Int32
    local i = keys.locate_last( name )
    if (not i.exists) return default

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_REAL64
        return reader.read_real64->Int32

      case DATA_TYPE_INT64
        return reader.read_int64x->Int32

      case DATA_TYPE_INT32, DATA_TYPE_LOGICAL
        return reader.read_int32x
    endWhich

    return default

  method logical( name:String, default=false:Logical )->Logical
    local i = keys.locate_last( name )
    if (not i.exists) return default

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_REAL64
        return reader.read_real64 != 0

      case DATA_TYPE_INT64
        return reader.read_int64x != 0

      case DATA_TYPE_INT32, DATA_TYPE_LOGICAL
        return reader.read_int32x != 0
    endWhich

    return default

  method bytes( name:String, bytes=Byte[]:Byte[] )->Byte[]
    local i = keys.locate_last( name )
    if (not i.exists) return bytes

    reader.seek( offsets[i.value] )
    which (reader.read_int32x)
      case DATA_TYPE_STRING
        local count = reader.read_int32x
        bytes.reserve( count )
        forEach (1..count) bytes.add( reader.read_int32x->Byte )

      case DATA_TYPE_BYTES
        local count = reader.read_int32x
        bytes.reserve( count )
        forEach (1..count) bytes.add( reader.read->Byte )
    endWhich

    return bytes

  method value( name:String, default=UndefinedValue:Value )->Value
    local json = string( name, null as String )
    if (not json) return default

    return JSON.parse( json )

  method list( name:String, default=UndefinedValue:Value )->Value
    local result = value( name )
    if (result.is_list) return result
    else                return default

  method table( name:String, default=UndefinedValue:Value )->Value
    local result = value( name )
    if (result.is_table) return result
    else                 return default

  method set( name:String, _value:String )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_STRING ).write_string( _value )
    return this

  method set( name:String, _value:Real64 )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_REAL64 ).write_real64( _value )
    return this

  method set( name:String, _value:Int64 )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_INT64 ).write_int64x( _value )
    return this

  method set( name:String, _value:Int32 )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_INT32 ).write_int32x( _value )
    return this

  method set( name:String, _value:Logical )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_LOGICAL ).write_logical( _value )
    return this

  method set( name:String, bytes:Byte[] )->this
    writer.write_string( name )
    writer.write_int32x( Message.DATA_TYPE_BYTES ).write_int32x( bytes.count )
    forEach (b in bytes) writer.write_byte( b )
    return this

  method set( name:String, _value:Value )->this
    return set( name, _value.to_json )

  method send
    MessageManager.send( this )
    sent = true

  method send_rsvp( callback:Function(Message) )
    # This form of send_rsvp waits for the native layer to reply() directly to this
    # message.
    MessageManager.send_rsvp( this, callback )
    sent = true

  method send_rsvp( response_message_type:String, callback:Function(Message) )
    # Rather than awaiting a reply to this specific message, this form of send_rsvp
    # installs a global one-time message listener that calls the given callback
    # when a new message (not a reply) of the given response type comes through.
    add_message_listener( response_message_type, callback, &once )
    MessageManager.send( this )
    sent = true

  # INTERNAL USE ------------------------------------------------------------

  method index_another->Logical
    if (not reader.has_another) return false
    local key    = read_type_name
    local offset = reader.position
    keys.add( key )
    offsets.add( offset )

    # Skip value
    local data_type = reader.read_int32x
    which (data_type)
      case DATA_TYPE_STRING
        local count = reader.read_int32x
        forEach (1..count) reader.read_int32x

      case DATA_TYPE_BYTES
        local count = reader.read_int32x
        forEach (1..count) reader.read_byte

      case DATA_TYPE_REAL64
        reader.read_real64

      case DATA_TYPE_INT64
        reader.read_int64x

      case DATA_TYPE_INT32, DATA_TYPE_LOGICAL
        reader.read_int32x

      others
        println "ERROR: unsupported message data type $." (data_type)
    endWhich
    return true

  method read_type_name->String
    return MessageManager.consolidated_type_name( reader.read_string(MessageManager.string_buffer.clear) )

endClass

}

