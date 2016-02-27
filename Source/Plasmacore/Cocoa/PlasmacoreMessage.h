#import <Cocoa/Cocoa.h>

#include "SuperCPPDataBuilder.h"
#include "SuperCPPList.h"

#import "PlasmacoreCallback.h"
#import "PlasmacoreMessageManager.h"

@interface PlasmacoreMessage : NSObject
{
  id                        manager;
  bool                      is_incoming;
  SuperCPP::List<int>       keys;
  SuperCPP::List<int>       offsets;
  SuperCPP::DataBuilder     buffer;
}

@property (nonatomic,readonly) int   typeID;
@property (nonatomic,readonly) int   messageID;
@property (nonatomic,readonly) char* data;
@property (nonatomic,readonly) int   size;

+ (id)         messageWithType:(const char*)message_type;
+ (id)         messageWithType:(const char*)message_type withMessageID:(int)m_id;

- (id)         initWithManager:(id)_manager;
- (id)         reset;
- (void)       decodeIncomingMessageData:(char*)data size:(int)size;

- (NSString*)  messageType;
- (int)        messageID;

// Outgoing Message API
- (PlasmacoreMessage*)  createReply;
- (void)                push;      // sends the message and has the manager dispatch pending messages
- (void)                pushRSVP:(PlasmacoreCallback)callback;
- (void)                send;
- (void)                sendRSVP:(PlasmacoreCallback)callback;

- (char*)               data;
- (int)                 size;

- (PlasmacoreMessage*)  setCString: (const char*)name value:(const char*)value;
- (PlasmacoreMessage*)  setString:  (const char*)name value:(NSString*)value;
- (PlasmacoreMessage*)  setReal64:  (const char*)name value:(double)value;
- (PlasmacoreMessage*)  setInt64:   (const char*)name value:(SuperCPP::Int64)value;
- (PlasmacoreMessage*)  setInt32:   (const char*)name value:(int)value;
- (PlasmacoreMessage*)  setLogical: (const char*)name value:(bool)value;
- (PlasmacoreMessage*)  setBytes:   (const char*)name value:(NSData*)value;

// Incoming Message API
- (bool)            contains:   (const char*)name;
- (NSString*)       getString:  (const char*)name;
- (double)          getReal64:  (const char*)name;
- (SuperCPP::Int64) getInt64:   (const char*)name;
- (int)             getInt32:   (const char*)name;
- (bool)            getLogical: (const char*)name;
- (NSData*)         getBytes:   (const char*)name;

@end
