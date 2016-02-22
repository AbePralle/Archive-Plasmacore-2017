#import "PlasmacoreMessageManager.h"
#import "RogueProgram.h"
using namespace SuperCPP;

//=============================================================================
//  PlasmacoreListenerInfo
//=============================================================================
@interface PlasmacoreListenerInfo : NSObject
{
  int                listener_id;
  NSString*          message_type;
  PlasmacoreCallback callback;
}

@property (nonatomic) int                listener_id;
@property (nonatomic) NSString*          message_type;
@property (nonatomic) PlasmacoreCallback callback;

- (id) initWithID:(int)listener_id withMessageType:(NSString*)message_type withCallback:(PlasmacoreCallback)callback;

@end

@implementation PlasmacoreListenerInfo
- (id) initWithID:(int)listener_id withMessageType:(NSString*)message_type withCallback:(PlasmacoreCallback)callback;
{
  if ( !(self = [super init]) ) return nil;

  self.listener_id = listener_id;
  self.message_type = message_type;
  self.callback = callback;

  return self;
}
@end


//=============================================================================
//  PlasmacoreMessageManager
//=============================================================================
@interface PlasmacoreMessageManager()
- (void) handleReply:(PlasmacoreMessage*)m;
@end

@implementation PlasmacoreMessageManager

- (id) init
{
  self = [super init];
  if ( !self ) return nil;

  message_pool = [[NSMutableArray alloc] init];
  listeners_by_type = [[NSMutableDictionary alloc] init];
  listeners_by_id   = [[NSMutableDictionary alloc] init];
  reply_callbacks_by_message_id = [[NSMutableDictionary alloc] init];
  next_callback_id = 1;

  [self addListenerForType:"<reply>"
    withCallback:^(int this_id,PlasmacoreMessage* m)
    {
      [self handleReply:m];
    }
  ];
}

- (int) addListenerForType:(const char*)type withCallback:(PlasmacoreCallback)callback
{
  @synchronized (self)
  {
    NSString* type_name = [NSString stringWithUTF8String:type];
    NSMutableArray* list = [listeners_by_type objectForKey:type_name];
    if ( !list )
    {
      list = [[NSMutableArray alloc] init];
      [listeners_by_type setObject:list forKey:type_name];
    }

    int listener_id = next_callback_id++;
    PlasmacoreListenerInfo* info = [PlasmacoreListenerInfo alloc];
    [info initWithID:listener_id withMessageType:type_name withCallback:callback];

    [list addObject:info];
    [listeners_by_id setObject:info forKey:[NSNumber numberWithInt:listener_id]];

    return listener_id;
  }
}

- (void) removeListenerByID:(int)listener_id
{
  @synchronized (self)
  {
    NSNumber* key = [NSNumber numberWithInt:key];
    PlasmacoreListenerInfo* info = [listeners_by_id objectForKey:key];
    if ( !info ) return;

    [listeners_by_id removeObjectForKey:key];
    NSMutableArray* list = [listeners_by_type objectForKey:info.message_type];
    [list removeObject:info];
  }
}

- (PlasmacoreMessage*) obtainMessage
{
  @synchronized (self)
  {
    if (message_pool.count) return [message_pool removeLastObject];
    return [[PlasmacoreMessage alloc] initWithManager:self];
  }
}

- (void) dispatchMessages
{
  @synchronized (self)
  {
    if (dispatching_messages)
    {
      _dispatchRequested = true;
      return;
    }
    dispatching_messages = true;

    // Build the header containing all the new message name/id mappings
    DataBuilder header;
    header.write_int32x( new_outgoing_ids.count );
    for (int i=0; i<new_outgoing_ids.count; ++i)
    {
      int _id = new_outgoing_ids[i];
      header.write_int32x( _id );
      header.write_string( outgoing_id_to_name[_id] );
    }
    new_outgoing_ids.clear();
    
    // Fetch Rogue-side buffer to store the message data and copy in the header
    // and message buffer
    RogueClassPlasmacore__MessageManager* mm =
      (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
    RogueByteList* list = mm->incoming_buffer;
    RogueByteList__clear( list );
    RogueByteList__reserve__Int32( list, header.count + message_buffer.count );
    memcpy( list->data->bytes, header.data, header.count );
    memcpy( list->data->bytes + header.count, message_buffer.data, message_buffer.count );
    list->count = header.count + message_buffer.count;
    message_buffer.clear();

    // Call Rogue MessageManager.update(), which sends back a reference to another byte
    // list containing messages to us.
    list = RoguePlasmacore__MessageManager__update( mm );
    if (list)
    {
      DataReader reader( list->data->bytes, list->count );

      // Process the header consisting of any new ID definitions
      int new_id_count = reader.read_int32x();
      for (int i=0; i<new_id_count; ++i)
      {
        int new_id = reader.read_int32x();
        StringBuilder buffer;
        reader.read_string( buffer );
        const char* new_name = buffer.as_c_string();
 
        incoming_name_to_id[ new_name ] = new_id;

        // 'incoming_name_to_id' makes a copy of the c string key so retrieve that again
        // to store in 'incoming_id_to_name', which doesn't make a copy.
        StringTableEntry<int>* entry = incoming_name_to_id.find( new_name );
        incoming_id_to_name[ new_id ] = entry->key;
      }

      if (reader.has_another())
      {
        // Recycle the same message object.
        PlasmacoreMessage* m = [self obtainMessage];

        while (reader.has_another())
        {
          // Extract the next message into a PlasmacoreMessage object.
          int size = reader.read_int32();
          if (size > reader.remaining()) size = reader.remaining();

          [m decodeIncomingMessageData:(char*)(reader.data+reader.position) size:size];
          reader.position += size;

          // Dispatch the message to any listeners
          NSMutableArray* list = [listeners objectForKey:[m messageType]];
          if (list)
          {
            for (int i=0; i<list.count; ++i)
            {
              PlasmacoreListenerInfo* info = [list objectAtIndex:i];
              info.callback( info.listener_id, m );
            }
          }

        }

        [message_pool addObject:[m reset]];
      }
    }
 
    dispatching_messages = false;
  } // end @synchronized
}

- (void) handleReply:(PlasmacoreMessage*)m
{
  @synchronized (self)
  {
    NSNumber* key = [NSNumber numberWithInt:[m messageID]];
    PlasmacoreCallback callback = [reply_callbacks_by_message_id objectForKey:key];
    if (callback)
    {
      [reply_callbacks_by_message_id removeObjectForKey:key];
      callback( 0, m );
    }
  }
}

- (void) send:(PlasmacoreMessage*)m
{
  @synchronized (this)
  {
    int size = m.size;
    message_buffer.reserve( size + 4 );
    message_buffer.print_int32( size );
    message_buffer.add( m.data, size );

    [message_pool addObject:[m reset]];
  }
}

- (void) sendRSVP:(PlasmacoreMessage*)m withCallback:(PlasmacoreCallback)callback
{
  @synchronized (this)
  {
    [reply_callbacks_by_message_id setObject:callback forKey:[m messageID]];
    [self send:m];
  }
}

@end
