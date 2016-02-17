//=============================================================================
//  Messaging.h
//=============================================================================
#ifndef MESSAGING_H
#define MESSAGING_H

#include "SuperCPPDataBuilder.h"
#include "SuperCPPDataReader.h"
#include "SuperCPPStringTable.h"
#include "SuperCPPStringBuilder.h"
#include "SuperCPPIntTable.h"
#include "SuperCPPList.h"
using namespace SuperCPP;

struct MessageManager;

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
  static const int DATA_TYPE_REAL64_LIST   = 7;
  static const int DATA_TYPE_INT32_LIST    = 8;
  static const int DATA_TYPE_BYTE_LIST     = 9;

  MessageManager* manager;
  bool            is_outgoing;
  int             serial_number;

  // Outgoing use only
  int             start_position;

  // Incoming use only
  const char*     type;

  Message( MessageManager* manager );
  Message( MessageManager* manager, DataReader* reader );
  ~Message();

  // Outgoing Message API
  void     send();
  Message& set_string( const char* name, const char* value );
  Message& set_string( const char* name, Character* characters, int count );
  Message& set_string( const char* name, StringBuilder& value );
  Message& set_real64( const char* name, Real64 value );
  Message& set_int64( const char* name, Int64 value );
  Message& set_int32( const char* name, int value );
  Message& set_logical( const char* name, bool value );
  Message& set_real64_list( const char* name, Real64* list, int count );
  Message& set_int32_list( const char* name, Int32* list, int count );
  Message& set_byte_list( const char* name, Byte* bytes, int count );
  Message& set_byte_list( const char* name, Builder<Byte>& bytes );

  // Incoming Message API
  bool     contains( const char* name );
  int      get_string( const char* name, StringBuilder& buffer );
  Real64   get_real64( const char* name, Real64 default_value=0 );
  Int64    get_int64( const char* name, Int64 default_value=0 );
  int      get_int32( const char* name, int default_value=0 );
  bool     get_logical( const char* name, bool default_value=false );
  int      get_byte_list( const char* name, Builder<Byte>& bytes );
  int      get_real64_list( const char* name, Builder<Real64>& list );
  int      get_int32_list( const char* name, Builder<Int32>& list );

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
  int              next_serial_number;

  DataBuilder      data;
  StringTable<int> outgoing_name_to_id;
  IntTable<char*>  incoming_id_to_name;
  DataReader*      reader;

  // Indexing for current incoming message
  List<char*>      keys;
  List<int>        offsets;

  MessageManager();
  ~MessageManager();

  void    dispach_messages();
  Message message( const char* name, int serial_number=-1 );

  // INTERNAL USE ONLY
  int      locate_key( const char* name );
};

#endif // MESSAGING_H
