#import <Cocoa/Cocoa.h>

#import "Plasmacore.h"
#import "Plasmacore.h"
using namespace SuperCPP;

typedef void (^CCListener)( int listener_id, id message );

@interface CCMessage : NSObject
{
  Plasmacore::Message  message;
}

+ (id)         create:(const char*)message_type;
+ (id)         createReply:(int)message_id;

- (id)         initWithPlasmacoreMessage:(Plasmacore::Message)m;
- (NSString*)  getType;
- (int)        getID;

// Outgoing Message API
- (CCMessage*) reply;
- (void)       push;      // sends the message and has the manager dispatch pending messages
- (int)        pushRSVP:(CCListener)callback;
- (void)       send;
- (int)        sendRSVP:(CCListener)callback;

- (CCMessage*) setString:  (const char*)name value:(NSString*)value;
- (CCMessage*) setReal64:  (const char*)name value:(double)value;
- (CCMessage*) setInt64:   (const char*)name value:(Int64)value;
- (CCMessage*) setInt32:   (const char*)name value:(int)value;
- (CCMessage*) setLogical: (const char*)name value:(bool)value;
- (CCMessage*) setBytes:   (const char*)name value:(NSData*)value;

// Incoming Message API
- (bool)       contains:   (const char*)name;
- (NSString*)  getString:  (const char*)name;
- (double)     getReal64:  (const char*)name;
- (Int64)      getInt64:   (const char*)name;
- (int)        getInt32:   (const char*)name;
- (bool)       getLogical: (const char*)name;
- (NSData*)    getBytes:   (const char*)name;

@end
