//=============================================================================
//  Messaging.cpp
//=============================================================================
#include "Messaging.h"
#include "RogueProgram.h"

#include <cstdio>
#include <cstring>
using namespace std;

namespace Messaging
{

//=============================================================================
//  Message
//=============================================================================
Message::Message( Manager* manager, int serial_number )
  : manager(manager), serial_number(serial_number)
{
  start_position = manager->data.count;
  manager->data.write_int32( 0 );  // placeholder for message size
  is_outgoing = true;
}

Message::Message( Manager* manager, DataReader* main_reader )
  : manager(manager)
{
  is_outgoing = false;
  int size = main_reader->read_int32();

  // Create another reader for just this message
  DataReader reader( main_reader->data, main_reader->position+size );
  reader.position = main_reader->position;
  main_reader->position += size;

  type = read_id( &reader );
  serial_number = reader.read_int32x();
  while (index_another(&reader)) {}
}

Message::~Message()
{
  if (is_outgoing) send();  // just in case; no effect if message already sent
}

Message Message::reply()
{
  return manager->message( "<reply>", serial_number );
}

bool Message::send()
{
  if ( !is_outgoing )
  {
    printf( "ERROR: native layer attempting to send() an incoming message.\n" );
    return false;
  }

  if (start_position == -1) return false;  // already sent

  int cur_pos = manager->data.count;
  manager->data.count = start_position;

  // Write the total size of this message, not including the Int32 size itself.
  manager->data.write_int32( (cur_pos - start_position) - 4 );

  manager->data.count = cur_pos;

  start_position = -1;
  return true;
}

bool Message::send_rsvp( Callback callback, void* context )
{
  if ( !send() ) return false;
  manager->reply_callbacks_by_serial_number[ serial_number ] = CallbackWithContext(callback,context);
  return true;
}


bool Message::contains( const char* name )
{
  if ( !confirm_incoming() ) return 0;
  return manager->locate_key( name ) != -1;
}

int Message::get_string( const char* name, StringBuilder& buffer )
{
  if ( !confirm_incoming() ) return 0;

  int i = manager->locate_key( name );
  if (i == -1) return 0;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_STRING:
    case DATA_TYPE_BYTE_LIST:
      return reader.read_string( buffer );

    default:
      return 0;
  }
}

Real64 Message::get_real64( const char* name, Real64 default_value )
{
  if ( !confirm_incoming() ) return default_value;

  int i = manager->locate_key( name );
  if (i == -1) return default_value;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_REAL64:
      return reader.read_real64();

    case DATA_TYPE_INT64:
      return reader.read_int64x();

    case DATA_TYPE_INT32:
    case DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return default_value;
  }
}

Int64 Message::get_int64( const char* name, Int64 default_value )
{
  if ( !confirm_incoming() ) return default_value;

  int i = manager->locate_key( name );
  if (i == -1) return default_value;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_REAL64:
      return (Int64) reader.read_real64();

    case DATA_TYPE_INT64:
      printf( "READING INT64X\n" );
      return reader.read_int64x();

    case DATA_TYPE_INT32:
    case DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return default_value;
  }
}

int Message::get_int32( const char* name, int default_value )
{
  if ( !confirm_incoming() ) return default_value;

  int i = manager->locate_key( name );
  if (i == -1) return default_value;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_REAL64:
      return (int) reader.read_real64();

    case DATA_TYPE_INT64:
      return (int) reader.read_int64x();

    case DATA_TYPE_INT32:
    case DATA_TYPE_LOGICAL:
      return reader.read_int32x();

    default:
      return default_value;
  }
}

bool Message::get_logical( const char* name, bool default_value )
{
  if ( !confirm_incoming() ) return default_value;

  int i = manager->locate_key( name );
  if (i == -1) return default_value;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_REAL64:
      return reader.read_real64() != 0;

    case DATA_TYPE_INT64:
      return reader.read_int64x() != 0;

    case DATA_TYPE_INT32:
    case DATA_TYPE_LOGICAL:
      return reader.read_int32x() != 0;

    default:
      return default_value;
  }
}

int Message::get_byte_list( const char* name, Builder<Byte>& bytes )
{
  if ( !confirm_incoming() ) return 0;

  int i = manager->locate_key( name );
  if (i == -1) return 0;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_STRING:
    case DATA_TYPE_BYTE_LIST:
    {
      int count = reader.read_int32x();
      for (int i=0; i<count; ++i)
      {
        bytes.add( reader.read_int32x() );
      }
      return count;
    }

    default:
      return 0;
  }
}

int Message::get_real64_list( const char* name, Builder<Real64>& list )
{
  if ( !confirm_incoming() ) return 0;

  int i = manager->locate_key( name );
  if (i == -1) return 0;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_REAL64_LIST:
    {
      int count = reader.read_int32x();
      for (int i=0; i<count; ++i)
      {
        list.add( reader.read_real64() );
      }
      return count;
    }

    default:
      return 0;
  }
}

int Message::get_int32_list( const char* name, Builder<Int32>& list )
{
  if ( !confirm_incoming() ) return 0;

  int i = manager->locate_key( name );
  if (i == -1) return 0;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_INT32_LIST:
    {
      int count = reader.read_int32x();
      for (int i=0; i<count; ++i)
      {
        list.add( reader.read_int32x() );
      }
      return count;
    }

    default:
      return 0;
  }
}

bool  Message::index_another( DataReader* reader )
{
  if ( !confirm_incoming() ) return false;
  if (not reader->has_another())  return false;

  manager->keys.add( read_id(reader) );
  manager->offsets.add( reader->position );

  int data_type = reader->read_int32x();
  switch (data_type)
  {
    case DATA_TYPE_ID_DEFINITION:
    case DATA_TYPE_ID:
      read_id( reader );
      return true;

    case DATA_TYPE_STRING:
    case DATA_TYPE_BYTE_LIST:
    {
      StringBuilder buffer;
      reader->read_string( buffer );
      return true;
    }

    case DATA_TYPE_REAL64:
      reader->read_real64();
      return true;

    case DATA_TYPE_INT64:
      reader->read_int64x();
      return true;

    case DATA_TYPE_INT32:
    case DATA_TYPE_LOGICAL:
      reader->read_int32x();
      return true;

    case DATA_TYPE_REAL64_LIST:
    {
      int count = reader->read_int32x();
      for (int i=0; i<count; ++i) reader->read_real64();
      return true;
    }

    case DATA_TYPE_INT32_LIST:
    {
      int count = reader->read_int32x();
      for (int i=0; i<count; ++i) reader->read_int32x();
      return true;
    }

    default:
      printf( "ERROR: unknown data type '%d' reading incoming message in native layer.\n", data_type );
      return false;
  }
}

char* Message::read_id( DataReader* reader )
{
  if ( !confirm_incoming() ) return (char*) "Undefined";

  switch (reader->read_int32x())
  {
    case DATA_TYPE_ID_DEFINITION:
    {
      int id = reader->read_int32x();
      StringBuilder buffer;
      reader->read_string( buffer );

      IntTableEntry<char*>* entry = manager->incoming_id_to_name.find( id );
      if (entry)
      {
        return entry->value;  // already defined
      }
      else
      {
        char* name = new char[ buffer.count + 1 ];
        strcpy( name, buffer.as_c_string() );
        manager->incoming_id_to_name[ id ] = name;
        return name;
      }
    }

    case DATA_TYPE_ID:
    {
      int id = reader->read_int32x();
      IntTableEntry<char*>* entry = manager->incoming_id_to_name.find( id );
      if (entry)
      {
        return entry->value;  // defined as expected
      }
      else
      {
        printf( "ERROR: undefined message type in native layer.\n" );
        return (char*) "Undefined";
      }
    }

    default:
      printf( "ERROR: native layer incoming message missing id.\n" );
      return (char*) "Undefined";
  }
}

bool Message::confirm_incoming()
{
  if ( !is_outgoing ) return true;

  printf( "ERROR: native layer attempting to write to an incoming message.\n" );
  return false;
}

bool Message::confirm_outgoing()
{
  if (is_outgoing) return true;

  printf( "ERROR: native layer attempting to read an outgoing message.\n" );
  return false;
}

Message& Message::write_id( const char* name )
{
  if ( !confirm_outgoing() ) return *this;
  if (start_position == -1) return *this;

  StringTableEntry<int>* entry = manager->outgoing_name_to_id.find( name );
  if (entry)
  {
    manager->data.write_int32x( DATA_TYPE_ID );
    manager->data.write_int32x( entry->value );
  }
  else
  {
    int id = manager->outgoing_name_to_id.count;
    manager->outgoing_name_to_id[ name ] = id;
    manager->data.write_int32x( DATA_TYPE_ID_DEFINITION );
    manager->data.write_int32x( id );
    manager->data.write_string( name );
  }
  return *this;
}

Message& Message::set_string( const char* name, const char* value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_STRING );
  manager->data.write_string( value );
  return *this;
}

Message& Message::set_string( const char* name, Character* characters, int count )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_STRING );
  manager->data.write_string( characters, count );
  return *this;
}

Message& Message::set_string( const char* name, StringBuilder& value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_STRING );
  manager->data.write_string( value.data, value.count );
  return *this;
}

Message& Message::set_real64( const char* name, Real64 value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_REAL64 );
  manager->data.write_real64( value );
  return *this;
}

Message& Message::set_int64( const char* name, Int64 value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_INT64 );
  manager->data.write_int64x( value );
  return *this;
}

Message& Message::set_int32( const char* name, int value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_INT32 );
  manager->data.write_int32x( value );
  return *this;
}

Message& Message::set_logical( const char* name, bool value )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_LOGICAL );
  manager->data.write_int32x( value ? 1 : 0 );
  return *this;
}

Message& Message::set_real64_list( const char* name, Real64* list, int count )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_REAL64_LIST );
  manager->data.write_int32x( count );
  for (int i=0; i<count; ++i)
  {
    manager->data.write_real64( list[i] );
  }
  return *this;
}

Message& Message::set_int32_list( const char* name, Int32* list, int count )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_INT32_LIST );
  manager->data.write_int32x( count );
  for (int i=0; i<count; ++i)
  {
    manager->data.write_int32x( list[i] );
  }
  return *this;
}

Message& Message::set_byte_list( const char* name, Byte* bytes, int count )
{
  if (start_position == -1 || !confirm_outgoing()) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_BYTE_LIST );
  manager->data.write_int32x( count );
  for (int i=0; i<count; ++i)
  {
    manager->data.write_byte( bytes[i] );
  }
  return *this;
}

Message& Message::set_byte_list( const char* name, Builder<Byte>& bytes )
{
  return set_byte_list( name, bytes.data, bytes.count );
}

//=============================================================================
//  Manager
//=============================================================================
Manager::Manager()
  : next_serial_number(1)
{
  add_listener( "<reply>", reply_handler, this );
}

Manager::~Manager()
{
  while (incoming_id_to_name.count)
  {
    delete incoming_id_to_name.remove_another();
  }

  while (listeners.count)
  {
    delete listeners.remove_another();
  }
}

void Manager::add_listener( const char* message_name, Callback listener, void* context )
{
  if ( !listeners.contains(message_name) )
  {
    listeners[ message_name ] = new List<CallbackWithContext>();
  }
  listeners[ message_name ]->add( CallbackWithContext(listener,context) );
}

void Manager::dispach_messages()
{
  // Copy message bytes into Rogue-side Manager.incoming_buffer.
  RogueClassPlasmacore__MessageManager* mm =
    (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
  RogueByteList* list = mm->incoming_buffer;
  RogueByteList__clear( list );
  RogueByteList__reserve__Int32( list, data.count );
  memcpy( list->data->bytes, data.data, data.count );
  list->count = data.count;

  data.clear();

  // Call Rogue Manager.update()
  list = RoguePlasmacore__MessageManager__update( mm );
  if (list)
  {
    DataReader reader( list->data->bytes, list->count );
    while (reader.has_another())
    {
      this->reader = &reader;
      keys.clear();
      offsets.clear();
      Message m( this, &reader );

//printf( "Native received message type %s\n", m.type );
      if (listeners.contains(m.type))
      {
        List<CallbackWithContext>* list = listeners[ m.type ];
        for (int i=list->count; --i >= 0; )
        {
          CallbackWithContext& info = list->data[i];
          info.callback( m, info.context );
        }
      }
    }
  }
}

Message Manager::message( const char* name, int serial_number )
{
  if (serial_number == -1) serial_number = next_serial_number++;
  Message result( this, serial_number );
  result.write_id( name );
  data.write_int32x( serial_number );
  return result;
}

void Manager::remove_listener( const char* message_name, Callback listener, void* context )
{
  if (listeners.contains(message_name))
  {
    List<CallbackWithContext>* list = listeners[ message_name ];
    for (int i=list->count; --i >= 0; )
    {
      CallbackWithContext& info = list->data[i];
      if (info.callback == listener && info.context == context)
      {
        list->remove_at( i );
        return;
      }
    }
  }
}


//-----------------------------------------------------------------------------
// INTERNAL
//-----------------------------------------------------------------------------
int Manager::locate_key( const char* name )
{
  for (int i=keys.count; --i >=0; )
  {
    if (0 == strcmp(keys[i],name)) return i;
  }
  return -1;
}

void Manager::reply_handler( Message m, void* context )
{
  Manager* manager = (Manager*) context;
  int      n = m.serial_number;
  if (manager->reply_callbacks_by_serial_number.contains(n))
  {
    CallbackWithContext& info = manager->reply_callbacks_by_serial_number[ n ];
    info.callback( m, info.context );
  }
}

} // namespace Messaging

