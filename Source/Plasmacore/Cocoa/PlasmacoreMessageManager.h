#import <Cocoa/Cocoa.h>

#include "SuperCPPIntTable.h"
#include "SuperCPPDataBuilder.h"
#include "SuperCPPDataReader.h"
#include "SuperCPPList.h"
#include "SuperCPPStringTable.h"


@interface PlasmacoreMessageManager : NSObject
{
  NSMutableArray*            message_pool;
  NSMutableDictionary*       listeners_by_type;
  NSMutableDictionary*       listeners_by_id;
  NSMutableDictionary*       reply_callbacks_by_message_id;
  int                        next_callback_id;

  SuperCPP::StringTable<int> outgoing_name_to_id;
  SuperCPP::StringTable<int> incoming_name_to_id;
  SuperCPP::IntTable<char*>  outgoing_id_to_name;
  SuperCPP::IntTable<char*>  incoming_id_to_name;
  SuperCPP::List<int>        new_outgoing_ids;
  SuperCPP::DataBuilder      message_buffer;
  bool                       dispatching_messages;
}

@property (nonatomic,readonly) bool dispatchRequested;

- (id)                 init;
- (int)                addListenerForType:(const char*)type withCallback:(PlasmacoreCallback)callback;
- (PlasmacoreMessage*) obtainMessage;
- (void)               dispatchMessages;
- (void)               send:(PlasmacoreMessage*)m;
- (void)               sendRSVP:(PlasmacoreMessage*)m withCallback:(PlasmacoreCallback)callback;
- (void)               removeListenerByID:(int)listener_id;

@end
