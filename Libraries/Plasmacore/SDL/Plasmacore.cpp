#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <string>
#include <cstdint>
#include <iostream>

#include "Plasmacore.h"
#include "PlasmacoreMessage.h"
#include "PlasmacoreView.h"
#include "RogueInterface.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <stdlib.h>
#else
#include <libgen.h>
#include <unistd.h>
#endif

//#define WINDOW_BASED 1

// The "code" value for SDL_USEREVENT for our async call mechanism
#define ASYNC_CALL_EVENT 1

static int gargc;
static char ** gargv;


static int iterations;
static volatile bool plasmacore_launched = false;


#ifdef __EMSCRIPTEN__
static void do_async_call ( void (*cb)(void *), int millis )
{
  emscripten_async_call(cb, 0, millis);
}

#else

static Uint32 sdl_async_cb_poster (Uint32 interval, void * arg)
{
  SDL_Event event;
  event.user.type = SDL_USEREVENT;
  event.user.code = ASYNC_CALL_EVENT;
  event.user.data1 = arg; // The callback
  //userevent.data2 = ...
  SDL_PushEvent(&event);
  return 0;
}

static void do_async_call ( void (*cb)(void *), int millis )
{
  SDL_AddTimer(millis, sdl_async_cb_poster, (void *)cb);
}

#endif


HID Plasmacore::addMessageHandler( std::string & type, HandlerCallback handler )
{
  auto info = new PlasmacoreMessageHandler( nextHandlerID, type, handler );
  nextHandlerID += 1;

  handlers_by_id[ info->handlerID ] = info;
  if ( !handlers[ type.c_str() ] ) handlers[ type.c_str() ] = new PlasmacoreList<PlasmacoreMessageHandler*>();
  handlers[ type.c_str() ]->add( info );
  return info->handlerID;
}


HID Plasmacore::addMessageHandler( const char * type, HandlerCallback handler )
{
  auto info = new PlasmacoreMessageHandler( nextHandlerID, type, handler );
  nextHandlerID += 1;

  handlers_by_id[ info->handlerID ] = info;
  if ( !handlers[ type ] ) handlers[ type ] = new PlasmacoreList<PlasmacoreMessageHandler*>();
  handlers[ type ]->add( info );
  return info->handlerID;
}


Plasmacore & Plasmacore::configure()
{
  if (is_configured) return *this;
  is_configured = true;

  addMessageHandler( "<reply>", [] (PlasmacoreMessage m)
    {
        auto entry = singleton.reply_handlers.find( m.message_id );
        if (entry)
        {
          auto handler = entry->value;
          singleton.reply_handlers.remove( m.message_id );
          handler->callback( m );
          delete handler;
        }
    }
  );

  #ifdef WINDOW_BASED
  addMessageHandler( "Window.create", [] (PlasmacoreMessage m)
    {
      auto name = m.getString( "name" );

      auto view = plasmacore_new_view(name);
      if (!view) throw "No view created!";

      Plasmacore::singleton.resources[ m.getInt32("id") ] = view;
      std::cerr << "Controller window: " << view << std::endl;
    }
  );

  addMessageHandler( "Window.show", [] (PlasmacoreMessage m)
    {
      auto window_id = m.getInt32( "id" );
      auto view  = (PlasmacoreView*)Plasmacore::singleton.resources[ window_id ];
      if (view) view->show();
    }
  );
  #else
  auto view = plasmacore_new_view("Main");
  if (!view) throw "No view created!";
  std::cerr << "Controller window: " << view << std::endl;
  #endif

  RogueInterface_set_arg_count( gargc );
  for (int i = 0; i < gargc; ++i)
  {
    RogueInterface_set_arg_value( i, gargv[i] );
  }

  RogueInterface_configure();
  return *this;
}


RID Plasmacore::getResourceID( void * resource)
{
  if (! resource) return 0;

  for (auto const & entry : resources)
  {
    if (entry->value == resource) { return entry->key; }
  }
  return 0;
}


Plasmacore & Plasmacore::launch()
{
  if (is_launched) { return *this; }
  is_launched = true;

  RogueInterface_launch();
  auto m = PlasmacoreMessage( "Application.on_launch" );
#ifdef WINDOW_BASED
  m.set( "is_window_based", true );
#endif
  m.send();
  return *this;
}




Plasmacore & Plasmacore::relaunch()
{
  //XXX: Always relaunch window based?
  PlasmacoreMessage( "Application.on_launch" ).set( "is_window_based", true ).send();
  return *this;
}


void Plasmacore::removeMessageHandler( HID handlerID )
{
  auto entry = handlers_by_id.find(handlerID);
  if (entry)
  {
    auto handler = entry->value;
    handlers_by_id.remove( handlerID );
    auto handler_list = handlers[ handler->type.c_str() ];
    if (handler_list)
    {
      for (int i = 0; i < handler_list->count; ++i)
      {
        if ((*handler_list)[i]->handlerID == handlerID)
        {
          handler_list->remove_at( i );
          return;
        }
      }
    }
    delete handler;
  }
}


void Plasmacore::send( PlasmacoreMessage & m )
{
  auto size = m.data.count;
  pending_message_data.add( uint8_t((size>>24)&255) );
  pending_message_data.add( uint8_t((size>>16)&255) );
  pending_message_data.add( uint8_t((size>>8)&255) );
  pending_message_data.add( uint8_t(size&255) );
  for (int i = 0; i < m.data.count; ++i)
  {
    pending_message_data.add( m.data[i] );
  }
  real_update(false);
}


void Plasmacore::send_rsvp( PlasmacoreMessage & m, HandlerCallback callback )
{
  reply_handlers[ m.message_id ] = new PlasmacoreMessageHandler( nextHandlerID, "<reply>", callback );
  nextHandlerID += 1;
  send( m );
}


Plasmacore & Plasmacore::setIdleUpdateFrequency( double f )
{
  idleUpdateFrequency = f;
  if (update_timer)
  {
    stop();
    start();
  }
  return *this;
}


void Plasmacore::start()
{
  if ( !is_launched ) { configure().launch(); }
  printf("LOG: start()\n");

  update_timer = true;
  real_update(true);
}


void Plasmacore::stop()
{
  // Note that we don't actually stop any timer from running.  The major
  // outcome here is that you may get an extra update if you stop, restart,
  // or change the update frequency.
  // We *could* probably make this work "right" by using a cancellable SDL
  // timer, but I don't think it really matters, and I'm not sure that such
  // timers are available in emscripten, so we'd have to do this for emscripten
  // mode anyway.
  update_timer = false;
}


void Plasmacore::update(void * dummy)
{
  //printf("LOG: update(); iterations=%i\n", iterations);
  Plasmacore::singleton.real_update(true);
}

void Plasmacore::fast_update(void * dummy)
{
  //printf("LOG: update(); iterations=%i\n", iterations);
  Plasmacore::singleton.real_update(false);
}

void Plasmacore::real_update (bool reschedule)
{
  if (!Plasmacore::singleton.update_timer) return; // Ignore, since timer shouldn't be running.

  // Schedule the next idle update.
  if (reschedule) do_async_call(update, int(1000*idleUpdateFrequency));

  if (is_sending)
  {
    update_requested = true;
    return;
  }

  is_sending = true;

  // Execute a small burst of message dispatching and receiving.  Stop after
  // 10 iterations or when there are no new messages.  Global state updates
  // are frequency capped to 1/60 second intervals and draws are synced to
  // the display refresh so this isn't triggering large amounts of extra work.
  for (int _ = 0; _ < 10; ++_)
  {
    update_requested = false;

    // Swap pending data with io_buffer data
    io_buffer.clear().add( pending_message_data );
    pending_message_data.clear();

    RogueInterface_send_messages( io_buffer );
    auto count = io_buffer.count;
    //if (count || io_buffer.size())
    //  printf("TX:%-8lu  RX:%-8lu  @iter:%i\n", io_buffer.size(), count, iterations);
    uint8_t * bytes = &io_buffer[0];

    int read_pos = 0;
    while (read_pos+4 <= count)
    {
      auto size = int( bytes[read_pos] ) << 24;
      size |= int( bytes[read_pos+1] ) << 16;
      size |= int( bytes[read_pos+2] ) << 8;
      size |= int( bytes[read_pos+3] );
      read_pos += 4;

      if (read_pos + size <= count)
      {
        decode_buffer.clear().reserve( size );
        for (int i = 0; i < size; ++i)
        {
          decode_buffer.add( bytes[read_pos+i] );
        }

        auto m = PlasmacoreMessage( decode_buffer );
        printf( "Received message type %s\n", m.type.characters );
        auto entry = handlers.find(m.type.characters);
        if (entry)
        {
          auto& list = *(entry->value);
          for (int i=0; i<list.count; ++i)
          {
            list[i]->callback( m );
          }
        }
      }
      else
      {
        std::cerr << "*** Skipping message due to invalid size.\n";
      }
      read_pos += size;
    }

    io_buffer.clear();

    if ( !update_requested )
    {
      break;
    }
  }

  is_sending = false;

  if (update_requested)
  {
    // There are still some pending messages after 10 iterations.  Schedule another round
    // in 1/60 second instead of the usual 1.0 seconds.
    // We'll now have another (slow) call to update, but it'll probably return
    do_async_call(fast_update, 16);
  }
}


static bool should_quit = false;


static void do_iteration (void)
{
  ++iterations;
  if ( !plasmacore_launched ) return;

  plasmacore_redraw_all_windows();
  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    switch (e.type)
    {
      case SDL_QUIT:
        should_quit = true;
        return;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      {
        auto w = plasmacore_get_window(e.button.windowID);
        if (!w) break;
        int which;
        switch (e.button.button)
        {
          case SDL_BUTTON_LEFT:
            which = 0;
            break;
          case SDL_BUTTON_RIGHT:
            which = 1;
            break;
          default:
            return;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN)
          w->on_mouse_down(e.button.x, e.button.y, which);
        else
          w->on_mouse_up(e.button.x, e.button.y, which);
        break;
      }
      case SDL_MOUSEMOTION:
      {
        auto w = plasmacore_get_window(e.motion.windowID);
        w->on_mouse_move(e.motion.x, e.motion.y);
        break;
      }
      case SDL_FINGERMOTION:
      {
        auto m = PlasmacoreMessage( "Input.on_stylus_event" );
        m.set( "type", 0 );  // 0=move
        m.set( "x", e.tfinger.x );
        m.set( "y", e.tfinger.y );
        m.send();
        break;
      }
      case SDL_FINGERDOWN:
      {
        auto m = PlasmacoreMessage( "Input.on_stylus_event" );
        m.set( "type", 1 );  // 1=press
        m.set( "x", e.tfinger.x );
        m.set( "y", e.tfinger.y );
        m.send();
        break;
      }
      case SDL_FINGERUP:
      {
        auto m = PlasmacoreMessage( "Input.on_stylus_event" );
        m.set( "type", 2 );  // 2=release
        m.set( "x", e.tfinger.x );
        m.set( "y", e.tfinger.y );
        m.send();
        break;
      }
      case SDL_WINDOWEVENT:
      {
        auto w = plasmacore_get_window(e.button.windowID);
        if (!w) break;
        switch (e.window.event)
        {
          case SDL_WINDOWEVENT_FOCUS_GAINED:
            w->on_focus_gained();
            break;
        }
        break;
      }
      case SDL_USEREVENT:
      {
        if (e.user.code == ASYNC_CALL_EVENT)
        {
          // The following cast is theoretically not allowed.  Let's hope
          // it works anyway on platforms we care about.
          void (*f) (void*) = (void (*)(void*))e.user.data1;
          f(0);
          break;
        }
      }
    }
  }
}

Plasmacore Plasmacore::singleton;

extern "C" void Rogue_sync_local_storage()
{
  #ifdef __EMSCRIPTEN__
  EM_ASM(
     FS.syncfs( false, function (err) {
       Module.print("Synching IDBFS");
     });
  );
  #endif
}

extern "C" void launch_plasmacore()
{
  Plasmacore::singleton.configure().launch();
  PlasmacoreMessage( "Application.on_start" ).send();
  Plasmacore::singleton.start();

  plasmacore_launched = true;
}

int main (int argc, char * argv[])
{
  gargc = argc;
  gargv = argv;

  #ifdef __EMSCRIPTEN__
    auto flags = 0;
  #else
    auto flags = SDL_INIT_TIMER;

    // CD into what we think the executable's directory is.
    char * exe = strdup(argv[0]);
    char * dir = dirname(exe);
    chdir(dir);
    free(exe);
  #endif

  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|flags) != 0) return 1;

  // Might want to call this here... but it might also be easier to just
  // call it from Rogue via "native".
  //SDL_ShowCursor(false);

#ifdef __EMSCRIPTEN__
  #ifdef LOCAL_FS
    plasmacore_launched = false;
    EM_ASM_({
       var mountpoint = Module["Pointer_stringify"]($0);
       FS.mkdir(mountpoint);
       FS.mount(IDBFS, {}, mountpoint);
       FS.syncfs(true, function (err) {
         Module.print("IDBFS ready");
         Module["_launch_plasmacore"]();
       });
    }, LOCAL_FS);
  #else
    launch_plasmacore();
  #endif
  emscripten_set_main_loop(do_iteration, 0, 1);
#else
  launch_plasmacore();

  SDL_GL_SetSwapInterval(1);
  while (!should_quit)
  {
    do_iteration();
    //SDL_Delay(20);
  }
#endif

  return 0;
}


