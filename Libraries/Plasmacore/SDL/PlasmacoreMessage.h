#ifndef PLASMACOREMESSAGE_H
#define PLASMACOREMESSAGE_H

#include <string>
#include <cstdint>
#include <map>

#include "PlasmacoreCString.h"
#include "PlasmacoreCStringBuilder.h"
#include "PlasmacoreList.h"
#include "PlasmacoreStringTable.h"

class PlasmacoreMessage;

typedef void (*HandlerCallback)(PlasmacoreMessage);


typedef int MID;
typedef int Int;
typedef std::string String;
typedef PlasmacoreList<uint8_t> Buffer;


class PlasmacoreMessage
{
public:
  enum DataType
  {
    STRING  = 1,
    REAL64  = 2,
    INT64   = 3,
    INT32   = 4,
    LOGICAL = 5,
    BYTES   = 6,
  };

  static MID next_message_id;

  PlasmacoreCString type;
  MID message_id;

  Buffer data;
  PlasmacoreStringTable<int> entries;
  int position = 0;

  PlasmacoreMessage ( Buffer& data );
  PlasmacoreMessage ( const char * type, MID message_id);
  PlasmacoreMessage ( const char * type );
  PlasmacoreMessage (void);
  void init (void);
  PlasmacoreMessage createReply (void) const;
  //Buffer getBytes (const char * name);
  int32_t getInt32 (const char* name, int32_t default_value = 0);
  int64_t getInt64 (const char* name, int64_t default_value = 0 );
  bool getLogical( const char* name, bool default_value = false );
  double getReal64( const char* name, double default_value=0.0 );
  void getString( const char* name, PlasmacoreCString& result );
  bool indexAnother (void);
  void send();
  void send_rsvp( HandlerCallback callback );
  PlasmacoreMessage & set( const char * name, Buffer & value );
  PlasmacoreMessage & set( const char * name, int64_t value );
  PlasmacoreMessage & set( const char * name, Int value );
  PlasmacoreMessage & set( const char * name, bool value );
  PlasmacoreMessage & set( const char * name, double value );
  PlasmacoreMessage & set( const char * name, const char * value );

private:
  //---------------------------------------------------------------------------
  // PRIVATE
  //---------------------------------------------------------------------------

  Int readByte (void);
  int64_t readInt64X (void);
  int32_t readInt32();
  Int readIntX();
  double readReal64 (void);
  void readString ( PlasmacoreCString& result );
  void readString ( PlasmacoreCStringBuilder& result );
  void writeByte ( Int value );
  void writeInt32( Int value );
  void writeInt64X( int64_t value );
  void writeIntX( Int value );
  void writeReal64 ( double value );
  void writeString ( const char* value );
};

#endif
