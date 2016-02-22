#import <Cocoa/Cocoa.h>

#include "SuperCPPDataBuilder.h"
#include "SuperCPPList.h"

typedef void (^PlamacoreCallback)( int this_id, id message );

@interface PlasmacoreMessage : NSObject
{
  PlasmacoreMessageManager* manager;
  bool                      is_incoming;
  int                       m_type;
  int                       m_id;
  SuperCPP::List<int>       keys;
  SuperCPP::List<int>       offsets;
  SuperCPP::DataBuilder     buffer;
}

@property (nonatomic) int m_type;
@property (nonatomic) int m_id;
@property (nonatomic,readonly) char* data;
@property (nonatomic,readonly) int   size;

+ (id)         messageWithType:(const char*)message_type;
+ (id)         messageWithReply:(int)message_id;

- (id)         initWithManager:(PlasmacoreMessageManager*)_manager;
- (id)         reset;
- (void)       decodeIncomingMessageData:(char*)data size:(int)size;

- (NSString*)  getType;
- (int)        getID;

// Outgoing Message API
- (PlasmacoreMessage*)  createReply;
- (void)       push;      // sends the message and has the manager dispatch pending messages
- (int)        pushRSVP:(CCListener)callback;
- (void)       send;
- (int)        sendRSVP:(CCListener)callback;

- (char*)      data;
- (int)        size;

- (PlasmacoreMessage*)  setString:  (const char*)name value:(NSString*)value;
- (PlasmacoreMessage*)  setReal64:  (const char*)name value:(double)value;
- (PlasmacoreMessage*)  setInt64:   (const char*)name value:(Int64)value;
- (PlasmacoreMessage*)  setInt32:   (const char*)name value:(int)value;
- (PlasmacoreMessage*)  setLogical: (const char*)name value:(bool)value;
- (PlasmacoreMessage*)  setBytes:   (const char*)name value:(NSData*)value;

// Incoming Message API
- (bool)       contains:   (const char*)name;
- (NSString*)  getString:  (const char*)name;
- (double)     getReal64:  (const char*)name;
- (Int64)      getInt64:   (const char*)name;
- (int)        getInt32:   (const char*)name;
- (bool)       getLogical: (const char*)name;
- (NSData*)    getBytes:   (const char*)name;

@end
