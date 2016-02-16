//=============================================================================
//  Messaging.cpp
//=============================================================================
#include "Messaging.h"
#include "RogueProgram.h"

#include <cstdio>
#include <cstring>
using namespace std;

//=============================================================================
//  Message
//=============================================================================
Message::Message( MessageManager* manager )
  : manager(manager)
{
  start_position = manager->data.count;
  manager->data.write_int32( 0 );  // placeholder for message size
  is_outgoing = true;
}

Message::Message( MessageManager* manager, DataReader* main_reader )
  : manager(manager)
{
  is_outgoing = false;
  int size = main_reader->read_int32();

  // Create another reader for just this message
  DataReader reader( main_reader->data, main_reader->position+size );
  reader.position = main_reader->position;
  main_reader->position += size;

  type = read_id( &reader );
printf( "Native layer received message %s\n", type );
  while (index_another(&reader)) {}
}

Message::~Message()
{
  if (is_outgoing) send();  // just in case; no effect if message already sent
}

void Message::send()
{
  if ( !is_outgoing )
  {
    printf( "ERROR: native layer attempting to send() an incoming message.\n" );
    return;
  }

  if (start_position == -1) return;  // already sent

  int cur_pos = manager->data.count;
  manager->data.count = start_position;

  // Write the total size of this message, not including the Int32 size itself.
  manager->data.write_int32( (cur_pos - start_position) - 4 );

  manager->data.count = cur_pos;

  start_position = -1;
}

int Message::get_int32( const char* name, int default_value )
{
  int i = manager->locate_key( name );
  if (i == -1) return default_value;

  DataReader reader( manager->reader->data + manager->offsets[i], manager->reader->count );
  switch (reader.read_int32x())
  {
    case DATA_TYPE_INT32:
      return reader.read_int32x();

    default:
      return default_value;
  }
}

bool  Message::index_another( DataReader* reader )
{
  if ( !require_outgoing(false) ) return false;
  if (not reader->has_another())  return false;

  manager->keys.add( read_id(reader) );
  manager->offsets.add( reader->position );
printf( "Indexing %s at %d\n", manager->keys.last(), manager->offsets.last() );

  int data_type = reader->read_int32x();
  switch (data_type)
  {
    case DATA_TYPE_ID_DEFINITION:
    case DATA_TYPE_ID:
      read_id( reader );
      return true;

    case DATA_TYPE_INT32:
      reader->read_int32x();
      return true;

    default:
      printf( "ERROR: unknown data type '%d' reading incoming message in native layer.\n", data_type );
      return false;
  }
}

char* Message::read_id( DataReader* reader )
{
  if ( !require_outgoing(false) ) return (char*) "Undefined";

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

bool Message::require_outgoing( bool flag )
{
  if (flag == is_outgoing) return true;

  if (flag)
  {
    printf( "ERROR: native layer attempting to read an outgoing message.\n" );
  }
  else
  {
    printf( "ERROR: native layer attempting to write to an incoming message.\n" );
  }
  return false;
}

Message& Message::write_id( const char* name )
{
  if ( !require_outgoing(true) ) return *this;
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

Message& Message::set_int32( const char* name, int value )
{
  if (start_position == -1) return *this;

  write_id( name );
  manager->data.write_int32x( DATA_TYPE_INT32 );
  manager->data.write_int32x( value );
  return *this;
}

//=============================================================================
//  MessageManager
//=============================================================================
MessageManager::~MessageManager()
{
  // Delete all entries in incoming_id_to_name table
  while (incoming_id_to_name.count)
  {
    delete incoming_id_to_name.remove_another();
  }
}

void MessageManager::dispach_messages()
{
  // Copy message bytes into Rogue-side MessageManager.incoming_buffer.
  RogueClassPlasmacore__MessageManager* mm =
    (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
  RogueByteList* list = mm->incoming_buffer;
  RogueByteList__clear( list );
  RogueByteList__reserve__Int32( list, data.count );
  memcpy( list->data->bytes, data.data, data.count );
  list->count = data.count;

  data.clear();

  // Call Rogue MessageManager.update()
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
printf( "message x:%d\n", m.get_int32("x") );
printf( "message z:%d\n", m.get_int32("z") );
    }
  }
}

Message MessageManager::message( const char* name )
{
  return Message( this ).write_id( name );
}

int MessageManager::locate_key( const char* name )
{
  for (int i=keys.count; --i >=0; )
  {
    if (0 == strcmp(keys[i],name)) return i;
  }
  return -1;
}

