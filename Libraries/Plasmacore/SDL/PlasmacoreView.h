#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#define INVALID_SDL_WID -1

class PlasmacoreView;

// Game authors can use the following to register their own views (including
// a custom default).  This allows you to change view stuff (like the window
// size) without actually editing this file.  Instead, add a file, subclass
// PlasmacoreView, and use a static initializer to call
// plasmacore_register_view (an example is shown below).  You're done!
#define DEFAULT_VIEW_FACTORY " "
typedef PlasmacoreView * (*ViewFactory)(const char*);
void plasmacore_register_view_factory (const char* name, ViewFactory factory);

// Here's how you can register factories from your own files without editing
// anything that comes with Plasmacore.
#if 0
#include "static_block.h"
static_block {
  // Register the default view factory
  // (For a named view, specify the name instead of DEFAULT_VIEW_FACTORY.)
  plasmacore_register_view_factory(DEFAULT_VIEW_FACTORY, [] (std::string & name) -> (PlasmacoreView *) {
    auto v = new MyDefaultPlasmacoreView();
    v->name = name;
    v->init();
    return v;
  );
};
#endif


PlasmacoreView * plasmacore_new_view ( const char* name );
PlasmacoreView * plasmacore_get_window (int swindow_id);
void plasmacore_redraw_all_windows (void);

class PlasmacoreView
{
public:
  bool isConfigured = false;
  const char* name  = "unnamed";
  int pwindowID     = 0; // Plasmacore windowID
  int swindowID     = INVALID_SDL_WID; // SDL windowID
  //var keyModifierFlags:UInt = 0
  SDL_Window * window;
  SDL_GLContext gl_context;
  int initial_width = 640;
  int initial_height = 480;
  int window_flags = SDL_WINDOW_SHOWN; //|SDL_WINDOW_FULLSCREEN;

  void destroy ();

  /// Override to change just window creation
  void create_window ();

  /// Called by view factory to create window
  void init ();

  void configure();
  void redraw ();

  /// Show the window
  void show ();

  // Stuff called by the SDL event dispatcher
  void on_mouse_up (int x, int y, int button);
  void on_mouse_down (int x, int y, int button);
  void on_mouse_move (int x, int y);
  void on_focus_gained  (void);
};
