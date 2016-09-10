//=============================================================================
//  PlasmacoreMessaging.h
//=============================================================================
#ifndef PLASMACORE_MESSAGING_H
#define PLASMACORE_MESSAGING_H

#include "SuperCPPDataBuilder.h"
#include "SuperCPPDataReader.h"
#include "SuperCPPStringTable.h"
#include "SuperCPPStringBuilder.h"
#include "SuperCPPIntTable.h"
#include "SuperCPPList.h"
using namespace SuperCPP;

namespace PLASMACORE
{

struct Message;
struct MessageManager;

//=============================================================================
//  Callback
//=============================================================================
typedef void (*Callback)( Message m, void* context, void* data );


//=============================================================================
//  CallbackWithContext
//=============================================================================
struct CallbackWithContext
{
  Callback callback;
  void*    context;
  void*    data;

  CallbackWithContext() : callback(0), context(0), data(0) {}

  CallbackWithContext( Callback callback, void* context, void* data )
    : callback(callback), context(context), data(data)
  {
  }
};

//=============================================================================
//  Message
//=============================================================================
struct Message
{
  static const int DATA_TYPE_ID_DEFINITION = 0;
  static const int DATA_TYPE_ID            = 1;
  static const int DATA_TYPE_STRING        = 2;
  static const int DATA_TYPE_REAL64        = 3;
  static const int DATA_TYPE_INT64         = 4;
  static const int DATA_TYPE_INT32         = 5;
  static const int DATA_TYPE_LOGICAL       = 6;
  static const int DATA_TYPE_BYTES         = 7;

  MessageManager* manager;
  bool            is_outgoing;
  int             id;

  // Outgoing use only
  int             start_position;

  // Incoming use only
  const char*     type;

  Message();
  Message( MessageManager* manager, int id );
  Message( MessageManager* manager, DataReader* reader );
  ~Message();

  // Outgoing Message API
  Message  create_reply();
  bool     push();      // sends the message and has the manager dispatch pending messages
  bool     push_rsvp( Callback callback, void* context=0, void* data=0 );
  bool     send();
  bool     send_rsvp( Callback callback, void* context=0, void* data=0 );
  Message& set_string( const char* name, const char* value );
  Message& set_string( const char* name, Character* characters, int count );
  Message& set_string( const char* name, StringBuilder& value );
  Message& set_real64( const char* name, Real64 value );
  Message& set_int64( const char* name, Int64 value );
  Message& set_int32( const char* name, int value );
  Message& set_logical( const char* name, bool value );
  Message& set_bytes( const char* name, Byte* bytes, int count );
  Message& set_bytes( const char* name, Builder<Byte>& bytes );

  // Incoming Message API
  bool     contains( const char* name );
  int      get_string( const char* name, StringBuilder& buffer );
  Real64   get_real64( const char* name, Real64 default_value=0 );
  Int64    get_int64( const char* name, Int64 default_value=0 );
  int      get_int32( const char* name, int default_value=0 );
  bool     get_logical( const char* name, bool default_value=false );
  int      get_bytes( const char* name, Builder<Byte>& bytes );

  // INTERNAL USE ONLY
  bool     index_another( DataReader* reader );
  char*    read_id( DataReader* reader );
  bool     confirm_incoming();
  bool     confirm_outgoing();
  Message& write_id( const char* name );
};

//=============================================================================
//  MessageManager
//=============================================================================
struct MessageManager
{
  // PROPERTIES
  int  next_id;
  bool dispatching;
  bool dispatch_requested;

  DataBuilder      data;
  StringTable<int> outgoing_name_to_id;
  IntTable<char*>  incoming_id_to_name;
  DataReader*      reader;

  // Indexing for current incoming message
  List<char*>      keys;
  List<int>        offsets;

  StringTable<List<CallbackWithContext>*>  listeners;
  IntTable<CallbackWithContext>            reply_callbacks_by_id;

  // METHODS
  MessageManager();
  ~MessageManager();

  void    add_listener( const char* message_name, Callback listener, void* context=0, void* data=0 );
  void    dispach_messages();
  Message create_message( const char* name, int id=-1 );
  Message create_reply( int id );
  void    remove_listener( const char* message_name, Callback listener, void* context=0 );


  // INTERNAL USE ONLY
  int         locate_key( const char* name );
  static void reply_handler( Message m, void* context, void* data );
};

} // namespace PLASMACORE

#endif // PLASMACORE_MESSAGING_H
