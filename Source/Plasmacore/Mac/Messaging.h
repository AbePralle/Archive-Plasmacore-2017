//=============================================================================
//  Messaging.h
//=============================================================================
#ifndef MESSAGING_H
#define MESSAGING_H

#include "SuperCPPDataBuilder.h"
#include "SuperCPPDataReader.h"
#include "SuperCPPStringTable.h"
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
  static const int DATA_TYPE_INT32         = 5;

  MessageManager* manager;
  bool            is_outgoing;

  // Outgoing use only
  int             start_position;

  // Incoming use only
  const char*     type;

  Message( MessageManager* manager );
  Message( MessageManager* manager, DataReader* reader );
  ~Message();

  // Outgoing Message API
  void     send();
  Message& write_int32( const char* name, int value );

  // Incoming Message API

  // INTERNAL USE ONLY
  char*    read_id( DataReader* reader );
  Message& write_id( const char* name );
};

//=============================================================================
//  MessageManager
//=============================================================================
struct MessageManager
{
  DataBuilder      data;
  StringTable<int> outgoing_name_to_id;
  IntTable<char*>  incoming_id_to_name;
  DataReader*      reader;

  // Indexing for current incoming message
  List<char*>      keys;
  List<int>        offsets;

  ~MessageManager();

  void    dispach_messages();
  Message message( const char* name );
};

#endif // MESSAGING_H
