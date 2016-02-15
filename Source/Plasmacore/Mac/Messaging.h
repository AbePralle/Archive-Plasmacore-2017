//=============================================================================
//  Messaging.h
//=============================================================================
#ifndef MESSAGING_H
#define MESSAGING_H

#include "SuperCPPDataBuilder.h"
#include "SuperCPPStringTable.h"
using namespace SuperCPP;

struct MessageManager;

//=============================================================================
//  Message
//=============================================================================
struct Message
{
  static const int INT32 = 0;

  MessageManager* manager;
  int             start_position;

  Message( MessageManager* manager );
  ~Message();

  void send();

  Message& write_id( const char* name );  // INTERNAL - don't call directly
  Message& write_int32( const char* name, int value );
};

//=============================================================================
//  MessageManager
//=============================================================================
struct MessageManager
{
  DataBuilder      data;
  StringTable<int> name_to_id;

  void    dispach_messages();
  Message message( const char* name );
};

#endif // MESSAGING_H
