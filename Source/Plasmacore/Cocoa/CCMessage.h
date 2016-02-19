#import <Cocoa/Cocoa.h>

#import "Plasmacore.h"
#import "Plasmacore.h"
using namespace SuperCPP;

typedef void (^CCListener)(id message);

@interface CCMessage : NSObject
{
  Plasmacore::Message  message;
}

- (id)         initWithPlasmacoreMessage:(Plasmacore::Message)m;
- (NSString*)  getType;

// Outgoing Message API
- (void)       push;      // sends the message and has the manager dispatch pending messages
- (void)       push_rsvp:(CCListener)callback;

/*
- (CCMessage*) reply;
- (void)       send;
- (void)       send_rsvp( Callback callback, void* context=0, void* data=0 );
- (Message*)   set_string( const char* name, const char* value );
- (Message*)   set_string( const char* name, Character* characters, int count );
- (Message*)   set_string( const char* name, StringBuilder& value );
- (Message*)   set_real64( const char* name, Real64 value );
- (Message*)   set_int64( const char* name, Int64 value );
- (Message*)   set_int32( const char* name, int value );
- (Message*)   set_logical( const char* name, bool value );
- (Message*)   set_real64_list( const char* name, Real64* list, int count );
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
