#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include <iostream>

#include "plasmacore.h"
#include "plasmacore_message.h"
#include "plasmacore_view.h"
#include "rogue_interface.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <libgen.h>
#include <unistd.h>
#endif

#define LOCAL_FS 1
//#define WINDOW_BASED 1

// The "code" value for SDL_USEREVENT for our async call mechanism
#define ASYNC_CALL_EVENT 1

static int gargc;
static char ** gargv;


static int iterations;


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
  auto info = PlasmacoreMessageHandler( nextHandlerID, type, handler );
  nextHandlerID += 1;

  handlers_by_id[ info.handlerID ] = info;
  handlers[ type ].push_back( info );
  return info.handlerID;
}


HID Plasmacore::addMessageHandler( const char * type, HandlerCallback handler )
{
  auto info = PlasmacoreMessageHandler( nextHandlerID, type, handler );
  nextHandlerID += 1;

  handlers_by_id[ info.handlerID ] = info;
  handlers[ type ].push_back( info );
  return info.handlerID;
}


Plasmacore & Plasmacore::configure()
{
  if (is_configured) return *this;
  is_configured = true;

  addMessageHandler( "<reply>", [] (PlasmacoreMessage m)
    {
        auto iter = singleton.reply_handlers.find( m.message_id );
        if (iter != singleton.reply_handlers.end())
        {
          auto info = iter->second;
          singleton.reply_handlers.erase(iter);
          info.callback( m );
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

  for (auto const & it : resources)
  {
    if (it.second == resource) { return it.first; }
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
  auto iter = handlers_by_id.find(handlerID);
  if (iter != handlers_by_id.end())
  {
    auto handler = iter->second;
    handlers_by_id.erase( handlerID );
    auto & handler_list = handlers[ handler.type ]; // This should always work, right?
    {
      for (int i = 0; i < handler_list.size(); ++i)
      {
        if (handler_list[i].handlerID == handlerID)
        {
          handler_list.erase(handler_list.begin() + i);
          return;
        }
      }
    }
  }
}


void Plasmacore::send( PlasmacoreMessage & m )
{
  auto size = m.data.size();
  pending_message_data.push_back( uint8_t((size>>24)&255) );
  pending_message_data.push_back( uint8_t((size>>16)&255) );
  pending_message_data.push_back( uint8_t((size>>8)&255) );
  pending_message_data.push_back( uint8_t(size&255) );
  for (int i = 0; i < m.data.size(); ++i)
  {
    pending_message_data.push_back( m.data[i] );
  }
  real_update(false);
}


void Plasmacore::send_rsvp( PlasmacoreMessage & m, HandlerCallback callback )
{
  reply_handlers[ m.message_id ] = PlasmacoreMessageHandler( nextHandlerID, "<reply>", callback );
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
    io_buffer.swap(pending_message_data);

    RogueInterface_send_messages( &io_buffer[0], io_buffer.size(), pending_message_data );
    auto count = pending_message_data.size();
    //if (count || io_buffer.size())
    //  printf("TX:%-8lu  RX:%-8lu  @iter:%i\n", io_buffer.size(), count, iterations);
    uint8_t * bytes = &pending_message_data[0];

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
        std::vector<uint8_t> message_data;
        message_data.reserve( size );
        for (int i = 0; i < size; ++i)
        {
          message_data.push_back( bytes[read_pos+i] );
        }

        auto m = PlasmacoreMessage( message_data );
        printf( "Received message type %s\n", m.type.c_str() );
        auto iter = handlers.find(m.type);
        if (iter != handlers.end())
        {
          for (auto const & handler : iter->second)
          {
            handler.callback( m );
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


extern "C" void start_main_loop (void)
{
  emscripten_set_main_loop(do_iteration, 0, 1);
}


Plasmacore Plasmacore::singleton;

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

  Plasmacore::singleton.configure().launch();
  PlasmacoreMessage( "Application.on_start" ).send();
  Plasmacore::singleton.start();

#ifdef __EMSCRIPTEN__
  #ifdef LOCAL_FS
    EM_ASM(
       FS.mkdir("/local_storage");
       FS.mount(IDBFS, {}, "/local_storage");
       FS.syncfs(true, function (err) {
         Module.print("Persistent storage ready.");
         Module["_start_main_loop"]();
       });
    );
  #else
    start_main_loop();
  #endif
#else
  SDL_GL_SetSwapInterval(1);
  while (!should_quit)
  {
    do_iteration();
    //SDL_Delay(20);
  }
#endif

  return 0;
}
