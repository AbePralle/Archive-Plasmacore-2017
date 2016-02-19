#import <Cocoa/Cocoa.h>

#import "Plasmacore.h"
#import "Plasmacore.h"
using namespace SuperCPP;

typedef void (^CCListener)( int listener_id, id message );

@interface CCMessage : NSObject
{
  Plasmacore::Message  message;
}

- (id)         initWithPlasmacoreMessage:(Plasmacore::Message)m;
- (NSString*)  getType;

// Outgoing Message API
- (CCMessage*) createReply;
- (void)       push;      // sends the message and has the manager dispatch pending messages
- (int)        pushRSVP:(CCListener)callback;
- (void)       send;
- (int)        sendRSVP:(CCListener)callback;
/*
- (Message*)   set_string( const char* name, const char* value );
- (Message*)   set_string( const char* name, Character* characters, int count );
- (Message*)   set_string( const char* name, StringBuilder& value );
- (Message*)   set_real64( const char* name, Real64 value );
- (Message*)   set_int64( const char* name, Int64 value );
- (Message*)   set_int32( const char* name, int value );
- (Message*)   set_logical( const char* name, bool value );
- (Message*)   set_bytes( const char* name, Byte* bytes, int count );
- (Message*)   set_bytes( const char* name, Builder<Byte>& bytes );
*/

// Incoming Message API
- (bool)       contains:   (NSString*)name;
- (NSString*)  getString:  (NSString*)name;
- (double)     getReal64:  (NSString*)name;
- (Int64)      getInt64:   (NSString*)name;
- (int)        getInt32:   (NSString*)name;
- (bool)       getLogical: (NSString*)name;
- (NSData*)    getBytes:   (NSString*)name;

@end
