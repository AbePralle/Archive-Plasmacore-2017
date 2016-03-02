#ifndef STARBRIGHT_H
#define STARBRIGHT_H
//=============================================================================
//  Starbright.h
//
//  HISTORY
//    2015.12.08 - Created by Abe Pralle.
//
//  See README.md for instructions.
//=============================================================================

#include "SuperCPP.h"
#include "SuperCPPResourceBank.h"

#ifndef PROJECT_WORKSPACE
  #define PROJECT_WORKSPACE Project
#endif

namespace PROJECT_WORKSPACE
{
namespace Starbright
{

// For built-in shaders
#if defined(CPP_PLATFORM_IOS) || defined(CPP_PLATFORM_ANDROID)
#  define SB_PRECISION_MEDIUMP_FLOAT "precision mediump float;\n"
#  define SB_LOWP " lowp "
#else
#  define SB_PRECISION_MEDIUMP_FLOAT ""
#  define SB_LOWP " "
#endif

// Notes on the following:
//   SB_TEXTURE_SHADER* works with both premultiplied alpha and regular textures.
//   SB_PREMULTIPLIED* only works with premultiplied alpha textures.
#define SB_COLOR_SHADER                                 1
#define SB_TEXTURE_SHADER                               2
#define SB_TEXTURE_SHADER_WITH_COLOR_MULTIPLY           3
#define SB_PREMULTIPLIED_TEXTURE_SHADER_WITH_COLOR_ADD  4
#define SB_PREMULTIPLIED_TEXTURE_SHADER_WITH_COLOR_FILL 5

#define SB_TEXTURE_OPTION_8BPP  8
#define SB_TEXTURE_OPTION_16BPP 16
#define SB_TEXTURE_OPTION_32BPP 32

//=============================================================================
//  RenderMode
//=============================================================================
struct RenderMode
{
  // CONSTANTS
  // dest_blend is (render_mode >> 28) & 15
  // src_blend  is (render_mode >> 24) & 15
  static const int BLEND_ZERO              = 0x00;
  static const int BLEND_ONE               = 0x01;
  static const int BLEND_SRC_ALPHA         = 0x02;
  static const int BLEND_INVERSE_SRC_ALPHA = 0x03;
  static const int BLEND_MASK              = 0x0f;

  static const int RENDER_BLEND            = 0x00001000;
  static const int RENDER_TEXTURES         = 0x00002000;
  static const int RENDER_TEXTURE_WRAP_H   = 0x00004000;
  static const int RENDER_TEXTURE_WRAP_V   = 0x00008000;
  static const int RENDER_TEXTURE_WRAP     = (RENDER_TEXTURE_WRAP_H | RENDER_TEXTURE_WRAP_V);
  static const int RENDER_POINT_FILTER     = 0x00010000;

  static const int VERTEX_COLOR_MULTIPLY   = 0x00100000;
  static const int VERTEX_COLOR_ADD        = 0x00200000;
  static const int VERTEX_COLOR_FILL       = 0x00300000;
  static const int VERTEX_COLOR_MODE_MASK  = 0x00f00000;

  static const int RENDER_OPTION_MASK      = 0x00FFF000;

  // PROPERTIES
  int value;

  // METHODS
  RenderMode()           : value(0) {}
  RenderMode( int value) : value(value) {}

  RenderMode( int src_blend, int dest_blend, int options=0 )
  {
    value = (dest_blend<<28) | (src_blend<<24) | options;
  }

  RenderMode enable_blend()          { return RenderMode( value | RENDER_BLEND ); }
  RenderMode enable_textures()       { return RenderMode( value | RENDER_TEXTURES ); }
  RenderMode enable_texture_wrap()   { return RenderMode( value | RENDER_TEXTURE_WRAP ); }
  RenderMode enable_point_filter()   { return RenderMode( value | RENDER_POINT_FILTER ); }
  RenderMode enable_color_multiply() { return RenderMode( value | VERTEX_COLOR_MULTIPLY ); }
  RenderMode enable_color_add()      { return RenderMode( value | VERTEX_COLOR_ADD ); }
  RenderMode enable_color_fill()     { return RenderMode( value | VERTEX_COLOR_FILL ); }

  bool blend_enabled() { return (value & RENDER_BLEND) != 0; }
  int  dest_blend() { return (value >> 28) & 15; }
  bool point_filter_enabled() { return (value & RENDER_POINT_FILTER) != 0; }
  bool textures_enabled() { return (value & RENDER_TEXTURES) != 0; }
  int  src_blend() { return (value >> 24) & 15; }
  bool wrap_h_enabled() { return (value & RENDER_TEXTURE_WRAP_H) != 0; }
  bool wrap_v_enabled() { return (value & RENDER_TEXTURE_WRAP_V) != 0; }
};

//=============================================================================
//  Shader
//=============================================================================
struct Shader
{
};

//=============================================================================
//  Texture
//=============================================================================
struct Texture
{
};

//=============================================================================
//  Matrix
//=============================================================================
typedef struct Matrix
{
  // PROPERTIES
  union
  {
    double at[16];
    struct
    {
      double r1c1;
      double r2c1;
      double r3c1;
      double r4c1;

      double r1c2;
      double r2c2;
      double r3c2;
      double r4c2;

      double r1c3;
      double r2c3;
      double r3c3;
      double r4c3;

      double r1c4;
      double r2c4;
      double r3c4;
      double r4c4;
    };
  };

  // METHODS
  Matrix();  // caution: garbage content
  Matrix( const Matrix& other );

  Matrix operator*( Matrix other );

  Matrix& set_identity();
  Matrix& set_orthographic( int left, int top, int right, int bottom, double near=-1, double far=1 );
  Matrix& set_zeros();
  Matrix  times( Matrix other );
  float*  to_float( float* dest );

  static Matrix identity();
  static Matrix projection( double left, double top, double right, double bottom, double near, double far );
  static Matrix translate( double tx, double ty, double tz );
  static Matrix zeros();

} Matrix;

//=============================================================================
//  Vertex
//=============================================================================
typedef struct Vertex
{
  // A renderer-agnostic vertex format used in buffered 2D drawing.
  SuperCPP::Real32 x, y, z;
  SuperCPP::Real32 u, v;
  SuperCPP::Int32  color;
} Vertex;

//=============================================================================
//  Renderer
//=============================================================================
struct Renderer
{
  static const int VERTEX_BUFFER_COUNT     = (512*3);
  static const int MAX_CONCURRENT_TEXTURES = 8;
  static const int TRIANGLES               = 0;
  static const int LINES                   = 1;
  static const int POINTS                  = 2;

  static const int CLEAR_COLOR             = 1;
  static const int CLEAR_DEPTH             = 2;
  static const int CLEAR_STENCIL           = 4;

  int        clear_color;
  double     clear_depth;
  int        clear_stencil;

  int        display_width;
  int        display_height;
  int        primitive_type;
  RenderMode render_mode;
  int        active_shader_id;
  int        active_texture_ids[MAX_CONCURRENT_TEXTURES];
  int        active_texture_count;

  // Shaders
  SuperCPP::ResourceBank<Shader*>  shaders;
  SuperCPP::ResourceBank<Texture*> textures;

  // Renderer-agnostic vertex buffer and render state that gets translated into
  // renderer-specific data when sb_render() is called.
  Vertex vertices[ VERTEX_BUFFER_COUNT ];
  int      vertex_count;

  Matrix projection_transform;

  // METHODS
  Renderer();
  virtual ~Renderer() {}

  virtual Renderer* activate();
  virtual Vertex*   add_vertices( int count );
  virtual void      begin_draw( int display_width, int display_height ) {}
  virtual void      end_draw() { flush(); }
  virtual void      clear( int flags ) {} // COLOR|DEPTH|STENCIL
  virtual int       define_shader( const char* vertex_src, const char* pixel_src ) { return 0; }
  virtual int       define_texture( void* pixels, int width, int height, int options ) { return 0; }
  virtual void      delete_shader( int shader_id ) {}
  virtual void      delete_texture( int texture_id ) {}
  virtual void      flush() {}
  virtual void      render() {}
  virtual void      set_clear_color( int color ) { clear_color = color; }
  virtual void      set_primitive_type( int type );
  virtual void      set_render_mode( RenderMode mode ) 
                    { if (render_mode.value != mode.value) render(); render_mode = mode; }
  virtual void      set_shader( int shader_id );
  virtual void      set_texture( int index, int texture_id );
  virtual void      set_texture_count( int texture_count );
  virtual void      set_transform_2d( double left, double top, double right, double bottom );
  virtual void      set_transform_2dx( double width, double height, double unit_z, double max_z, double tz=0 );
  virtual void      set_transform_3d( double left, double top, double right, double bottom, double near, double far );
};

extern Renderer* renderer;

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE

using namespace PROJECT_WORKSPACE;

#endif // STARBRIGHT_H
