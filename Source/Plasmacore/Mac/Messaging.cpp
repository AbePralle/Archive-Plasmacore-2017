//=============================================================================
//  Messaging.cpp
//=============================================================================
#include "Messaging.h"
#include "RogueProgram.h"


//=============================================================================
//  Message
//=============================================================================
Message::Message( MessageManager* manager ) : manager(manager)
{
  start_position = manager->data.count;
  manager->data.write_int32( 0 );  // placeholder for message size
}

Message::~Message()
{
  send();  // just in case; no effect if message already sent
}

void Message::send()
{
  if (start_position == -1) return;  // already sent

  int cur_pos = manager->data.count;
  manager->data.count = start_position;

  // Write the total size of this message, not including the Int32 size itself.
  manager->data.write_int32( (cur_pos - start_position) - 4 );

  manager->data.count = cur_pos;

  start_position = -1;
}

Message& Message::write_id( const char* name )
{
  if (start_position == -1) return *this;

  StringTableEntry<int>* entry = manager->name_to_id.find( name );
  if (entry)
  {
    manager->data.write_int32x( entry->value );
  }
  else
  {
    int id = manager->name_to_id.count;
    manager->name_to_id[ name ] = id;
    manager->data.write_int32x( id );
    manager->data.write_string( name );
  }
  return *this;
}

Message& Message::write_int32( const char* name, int value )
{
  if (start_position == -1) return *this;

  write_id( name );
  manager->data.write_int32x( INT32 );
  manager->data.write_int32x( value );
  return *this;
}

//=============================================================================
//  MessageManager
//=============================================================================
void MessageManager::dispach_messages()
{
  // Copy message bytes into Rogue-side MessageManager.receive_buffer.
  RogueClassPlasmacore__MessageManager* mm =
    (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
  RogueByteList* list = mm->receive_buffer;
  RogueByteList__clear( list );
  RogueByteList__reserve__Int32( list, data.count );
  memcpy( list->data->bytes, data.data, data.count );
  list->count = data.count;

  data.clear();

  // Call Rogue MessageManager.update()
  list = RoguePlasmacore__MessageManager__update( mm );
}

Message MessageManager::message( const char* name )
{
  return Message( this ).write_id( name );
}

