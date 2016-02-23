#import "PlasmacoreCallback.h"
#import "PlasmacoreMessageManager.h"
#import "RogueProgram.h"
using namespace SuperCPP;

//=============================================================================
//  PlasmacoreListenerInfo
//=============================================================================
@interface PlasmacoreListenerInfo : NSObject
{
  PlasmacoreCallback _callback;
}

@property (nonatomic) int                         listener_id;
@property (nonatomic) NSString*                   message_type;
@property (nonatomic,readonly) PlasmacoreCallback callback;

- (id) initWithID:(int)listener_id withMessageType:(NSString*)message_type withCallback:(PlasmacoreCallback)callback;

@end

@implementation PlasmacoreListenerInfo
- (id) initWithID:(int)listener_id withMessageType:(NSString*)message_type withCallback:(PlasmacoreCallback)callback;
{
  if ( !(self = [super init]) ) return nil;

  self.listener_id = listener_id;
  self.message_type = message_type;
  self->_callback = callback;

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

  __weak PlasmacoreMessageManager* manager = self;
  [self addListenerForType:"<reply>"
    withCallback:^(int this_id,PlasmacoreMessage* m)
    {
      [manager handleReply:m];
    }
  ];
  return self;
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
    info = [info initWithID:listener_id withMessageType:type_name withCallback:callback];

    [list addObject:info];
    [listeners_by_id setObject:info forKey:[NSNumber numberWithInt:listener_id]];

    return listener_id;
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
    RogueByteList* list = mm->io_buffer;
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
          NSMutableArray* list = [listeners_by_type objectForKey:[m messageType]];
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

    Rogue_collect_garbage( false );  // optional GC
 
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

- (const char*) incomingIDToName:(int)_id
{
  @synchronized (self)
  {
    return incoming_id_to_name[ _id ];
  }
}

- (int) incomingNameToID:(const char*)name
{
  @synchronized (self)
  {
    StringTableEntry<int>* entry = incoming_name_to_id.find( name );
    if (entry) return entry->value;
    return 0;
  }
}

- (PlasmacoreMessage*) obtainMessage
{
  @synchronized (self)
  {
    if (message_pool.count)
    {
      PlasmacoreMessage* m = [message_pool lastObject];
      [message_pool removeLastObject];
      return m;
    }
    return [[PlasmacoreMessage alloc] initWithManager:self];
  }
}

- (const char*) outgoingIDToName:(int)_id
{
  @synchronized (self)
  {
    return outgoing_id_to_name[ _id ];
  }
}

- (int) outgoingNameToID:(const char*)name
{
  @synchronized (self)
  {
    StringTableEntry<int>* entry = outgoing_name_to_id.find( name );
    if (entry) return entry->value;

    // Create a new id.  Since 'outgoing_name_to_id' creates a permanent
    // copy of the name string as a key, we'll re-used that key pointer
    // with outgoing_id_to_name.
    int new_id = outgoing_id_to_name.count + 1;
    outgoing_name_to_id[ name ] = new_id;
    outgoing_id_to_name[ new_id ] = outgoing_name_to_id.find(name)->key;
    new_outgoing_ids.add( new_id );
    return new_id;
  }
}

- (void) send:(PlasmacoreMessage*)m
{
  @synchronized (self)
  {
    int size = m.size;
    message_buffer.reserve( size + 4 );
    message_buffer.write_int32( size );
    message_buffer.add( (Byte*) m.data, size );

    [message_pool addObject:[m reset]];
  }
}

- (void) removeListenerWithID:(int)listener_id
{
  @synchronized (self)
  {
    NSNumber* key = [NSNumber numberWithInt:listener_id];
    PlasmacoreListenerInfo* info = [listeners_by_id objectForKey:key];
    if ( !info ) return;

    [listeners_by_id removeObjectForKey:key];
    NSMutableArray* list = [listeners_by_type objectForKey:info.message_type];
    [list removeObject:info];
  }
}

- (void) sendRSVP:(PlasmacoreMessage*)m withCallback:(PlasmacoreCallback)callback
{
  @synchronized (self)
  {
    [reply_callbacks_by_message_id setObject:callback forKey:[NSNumber numberWithInt:[m messageID]]];
    [self send:m];
  }
}

@end
