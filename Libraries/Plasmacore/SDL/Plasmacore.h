#ifndef PLASMACORE_H
#define PLASMACORE_H

#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <iostream>

#include "PlasmacoreList.h"
#include "PlasmacoreMessage.h"

typedef int HID;
typedef int RID;
typedef int Int;
typedef std::string String;
typedef PlasmacoreList<uint8_t> Buffer;



class PlasmacoreMessageHandler
{
public:
  HID handlerID;
  std::string type;
  HandlerCallback callback;

  PlasmacoreMessageHandler (HID handlerID, std::string & type, HandlerCallback callback)
  : handlerID(handlerID), type(type), callback(callback)
  {
  }

  PlasmacoreMessageHandler (HID handlerID, const char * type, HandlerCallback callback)
  : handlerID(handlerID), type(type), callback(callback)
  {
  }

  PlasmacoreMessageHandler ()
  : handlerID(-1), type("<invalid>")
  {
  }
};



class Plasmacore
{
public:
  static Plasmacore singleton;

  bool is_configured = false;
  bool is_launched   = false;

  HID nextHandlerID = 1;
  double idleUpdateFrequency = 0.5;

  Buffer pending_message_data;
  Buffer io_buffer;
  Buffer decode_buffer;

  bool is_sending = false;
  bool update_requested = false;

  std::map<std::string, std::vector<PlasmacoreMessageHandler>> handlers;
  std::map<HID, PlasmacoreMessageHandler> handlers_by_id;
  std::map<HID, PlasmacoreMessageHandler> reply_handlers;
  std::map<RID, void *> resources;

  bool update_timer = false; // true if running

  HID addMessageHandler( std::string & type, HandlerCallback handler );
  HID addMessageHandler( const char * type, HandlerCallback handler );
  Plasmacore & configure();
  RID getResourceID( void * resource);
  Plasmacore & launch();
  Plasmacore & relaunch();
  void removeMessageHandler( HID handlerID );
  void send( PlasmacoreMessage & m );
  void send_rsvp( PlasmacoreMessage & m, HandlerCallback callback );
  Plasmacore & setIdleUpdateFrequency( double f );
  void start();
  void stop();
  static void update(void * dummy);
  static void fast_update(void * dummy);
  void real_update(bool reschedule);
};

#endif
