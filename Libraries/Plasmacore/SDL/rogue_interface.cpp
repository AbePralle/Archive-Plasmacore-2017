#include "rogue_interface.h"
#include "RogueProgram.h"
#include "unistd.h"


#define ROGUEAPI

static int          RogueInterface_argc = 0;
static const char** RogueInterface_argv = {0};


ROGUEAPI RogueString* Plasmacore_get_documents_folder()
{
  return RogueString_validate(RogueString_create_from_utf8("."));
}

ROGUEAPI RogueString* Plasmacore_get_library_folder()
{
  return RogueString_validate(RogueString_create_from_utf8("."));
}

extern "C" RogueString* Plasmacore_find_asset( RogueString* filepath )
{
  if (access((char *)filepath->utf8, R_OK) == -1) return 0;
  return RogueString_create_from_utf8((char *)filepath->utf8);
}



ROGUEAPI void RogueInterface_configure()
{
  Rogue_configure( RogueInterface_argc, RogueInterface_argv );
  PlasmacoreSoundInit();
}

ROGUEAPI void RogueInterface_launch()
{
  Rogue_launch();
}

ROGUEAPI void RogueInterface_send_messages( const unsigned char* data, int count, std::vector<uint8_t> rx )
{
  try
  {
    Rogue_collect_garbage();

    RogueClassPlasmacore__MessageManager* mm =
      (RogueClassPlasmacore__MessageManager*) ROGUE_SINGLETON(Plasmacore__MessageManager);
    RogueByte_List* list = mm->io_buffer;

    RogueByte_List__clear( list );
    RogueByte_List__reserve__Int32( list, count );
    memcpy( list->data->as_bytes, data, count );
    list->count = count;

    // Call Rogue MessageManager.update(), which sends back a reference to another byte
    // list containing messages to us.
    list = RoguePlasmacore__MessageManager__update( mm );

    if ( !list )
    {
      rx.clear();
      return;
    }
    rx.assign(list->data->as_bytes+list->count, list->data->as_bytes);
  }
  catch (RogueException* err)
  {
    RogueException__display( err );
    rx.clear();
    return;
  }
}

ROGUEAPI void RogueInterface_set_arg_count( int count )
{
  // Note that this will leak the old argv if app is reconfigured
  RogueInterface_argc = count;
  RogueInterface_argv = new const char*[ count+1 ];
  memset( RogueInterface_argv, 0, sizeof(const char*) * (count+1) );
}

ROGUEAPI void RogueInterface_set_arg_value( int index, const char* value )
{
  size_t len = strlen( value );
  char* copy = new char[ len+1 ];
  strcpy( copy, value );
  RogueInterface_argv[ index ] = copy;
}
