#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Plasmacore.h"
#include "PlasmacoreUtility.h"
#include "PlasmacoreView.h"

PlasmacoreIntTable<PlasmacoreView*> sdl_windows;

const char* PlasmacoreView::default_window_title = "Plasmacore";
int         PlasmacoreView::default_display_width = 1024;
int         PlasmacoreView::default_display_height = 768;

PlasmacoreView * plasmacore_get_window (int swindow_id)
{
  if ( !sdl_windows.contains(swindow_id)) return 0;
  return sdl_windows[swindow_id];
}

void plasmacore_redraw_all_windows (void)
{
  for (auto const & iter : sdl_windows)
  {
    iter->value->redraw();
  }
}

void PlasmacoreView::destroy ()
{
  //TODO
  //SDL_GL_DeleteContext(gl_context);
  //sdl_windows.erase swindowID
}

/// Override to change just window creation
void PlasmacoreView::create_window ()
{
  window = SDL_CreateWindow(default_window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, initial_width, initial_height, window_flags|SDL_WINDOW_OPENGL);
}

/// Called by view factory to create window
void PlasmacoreView::init ()
{
  printf("LOG: PlasmacoreView::init()\n");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  create_window();
  if (!window) throw "Didn't create window";

  gl_context = SDL_GL_CreateContext(window);
  if (!gl_context) throw "Didn't create GL context";

  swindowID = SDL_GetWindowID(window);
  sdl_windows[swindowID] = this;

  if (SDL_GL_MakeCurrent(window, gl_context))
  {
    fprintf( stderr, "SDL_GL_MakeCurrent() failed: %s\n", SDL_GetError() );
    return;
  }

  glClearColor(1,1,0,1);
  glClear(GL_COLOR_BUFFER_BIT);

  configure(); // Is there a reason not to call this here?
}


void PlasmacoreView::configure()
{
  if (isConfigured) { return; }
  isConfigured = true;

  pwindowID = Plasmacore::singleton.getResourceID( this );

  SDL_RaiseWindow(window); // Should we immediately send a focus event? (Mac version does...)

  fprintf( stderr, "PlasmacoreView %s:\n", name );
  fprintf( stderr, "  SDL   WID %d:\n", swindowID );
  fprintf( stderr, "  PCore WID %d:\n", pwindowID );
}


void PlasmacoreView::redraw ()
{
  configure();
  if (!window) return;
  if (!gl_context) return;
  auto flags = SDL_GetWindowFlags(window);
  if (flags & SDL_WINDOW_MINIMIZED) return;
  if (!(flags & SDL_WINDOW_SHOWN)) return;

  if (SDL_GL_MakeCurrent(window, gl_context))
  {
    fprintf( stderr, "SDL_GL_MakeCurrent() failed: %s\n", SDL_GetError() );
    return;
  }

  int display_width, display_height;
  SDL_GetWindowSize(window, &display_width, &display_height);
  auto m = PlasmacoreMessage( "Display.on_render" );
  m.set( "window_id", pwindowID ).set( "display_name", name );
  m.set( "display_width",  display_width );
  m.set( "display_height", display_height );
  m.set( "viewport_width",  display_width );
  m.set( "viewport_height", display_height );
  m.send();
  SDL_GL_SwapWindow(window);
}


void PlasmacoreView::show ()
{
  SDL_ShowWindow(window);
}

void PlasmacoreView::on_mouse_down (int x, int y, int button)
{
  configure();
  // button 0 = left, 1 = right
  auto m = PlasmacoreMessage( "Display.on_pointer_event" );
  m.set( "window_id", pwindowID ).set( "display_name", name );
  m.set( "type", 1 );  // 1=press
  m.set( "x", x );
  m.set( "y", y );
  m.set( "index", button );
  m.send();
}

void PlasmacoreView::on_mouse_up (int x, int y, int button)
{
  configure();
  auto m = PlasmacoreMessage( "Display.on_pointer_event" );
  m.set( "window_id", pwindowID ).set( "display_name", name );
  m.set( "type", 2 );  // 2=release
  m.set( "x", x );
  m.set( "y", y );
  m.set( "index", button );
  m.send();
}

void PlasmacoreView::on_mouse_move (int x, int y)
{
  configure();
  auto m = PlasmacoreMessage( "Display.on_pointer_event" );
  m.set( "window_id", pwindowID ).set( "display_name", name );
  m.set( "type", 0 );  // 0=move
  m.set( "x", x );
  m.set( "y", y );
  m.send();
}

void PlasmacoreView::on_focus_gained  (void)
{
  configure();
  auto m = PlasmacoreMessage( "Display.focus_gained" );
  m.set( "window_id", pwindowID );
  m.set( "display_name", name );
  m.send();
}



PlasmacoreStringTable<ViewFactory>* plasmacore_views = 0;

void plasmacore_register_view_factory (const char* name, ViewFactory factory)
{
  if (!plasmacore_views) plasmacore_views = new PlasmacoreStringTable<ViewFactory>();
  (*plasmacore_views)[name] = factory;
}

static PlasmacoreView * default_factory (const char* name)
{
  auto v = new PlasmacoreView();
  v->name = name;
  v->init();
  return v;
}

PlasmacoreView * plasmacore_new_view ( const char* name )
{
  ViewFactory factory = 0;
  if ( (!plasmacore_views) || !plasmacore_views->contains(name) )
  {
    factory = default_factory;
  }
  else if (plasmacore_views->contains(name))
  {
    factory = (*plasmacore_views)[name];
  }
  else if (plasmacore_views->contains(DEFAULT_VIEW_FACTORY))
  {
    factory = (*plasmacore_views)[DEFAULT_VIEW_FACTORY];
  }
  if (!factory) throw "Null view factory?";
  return factory(name);
}

