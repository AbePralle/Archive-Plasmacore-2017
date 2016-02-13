#ifndef STARBRIGHT_OPEN_GL_RENDERER_H
#define STARBRIGHT_OPEN_GL_RENDERER_H
//=============================================================================
//  SBGLRenderer.h
//
//  HISTORY
//    2015.12.08 - Created by Abe Pralle.
//
//  See README.md for instructions.
//=============================================================================

#include "Starbright.h"

#if CPP_PLATFORM_MAC
  #include <OpenGL/gl.h>
#endif

#include <cstring>

#ifndef PROJECT_WORKSPACE
  #define PROJECT_WORKSPACE Project
#endif

namespace PROJECT_WORKSPACE
{
namespace Starbright
{

//-----------------------------------------------------------------------------
//  OpenGLShader
//-----------------------------------------------------------------------------
struct OpenGLShader : Shader
{
  int vertex_shader;
  int pixel_shader;
  int program;

  int transform_setting;
  int position_attribute;
  int color_attribute;

  // Only valid for texturing shaders
  int texture_settings[Renderer::MAX_CONCURRENT_TEXTURES];
  int uv_attribute;

  OpenGLShader()
  {
    memset( this, 0, sizeof(OpenGLShader) );
  }
};



//-----------------------------------------------------------------------------
//  OpenGLVertexXY
//-----------------------------------------------------------------------------
typedef struct OpenGLVertexXY
{
  GLfloat x, y;
} OpenGLVertexXY;


//-----------------------------------------------------------------------------
//  OpenGLVertexUV
//-----------------------------------------------------------------------------
typedef struct OpenGLVertexUV
{
  GLfloat u, v;
} OpenGLVertexUV;


//-----------------------------------------------------------------------------
//  OpenGLTexture
//-----------------------------------------------------------------------------
struct OpenGLTexture : Texture
{
  GLuint gl_id;
  int    width, height;
  int    options;

  OpenGLTexture()
  {
    memset( this, 0, sizeof(OpenGLTexture) );
  }
};

//-----------------------------------------------------------------------------
//  OpenGLRenderer
//-----------------------------------------------------------------------------
struct OpenGLRenderer : Renderer
{
  int color_shader;
  int texture_shader;
  int texture_shader_with_color_multiply;
  int premultiplied_texture_shader_with_color_add;
  int premultiplied_texture_shader_with_color_fill;

  OpenGLVertexXY vertex_buffer_xy[Renderer::VERTEX_BUFFER_COUNT];
  OpenGLVertexUV vertex_buffer_uv[Renderer::VERTEX_BUFFER_COUNT];
  GLint            color_buffer[Renderer::VERTEX_BUFFER_COUNT];

  OpenGLRenderer();
  ~OpenGLRenderer();

  void begin_draw( int display_width, int display_height );
  void end_draw();
  void clear( int flags ); // COLOR|DEPTH|STENCIL
  int  define_shader( const char* vertex_src, const char* pixel_src );
  int  define_texture( void* pixels, int width, int height, int options );
  void delete_shader( int shader_id );
  void delete_texture( int texture_id );
  void flush();
  void render();
};

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE
using namespace PROJECT_WORKSPACE;

#endif // STARBRIGHT_OPEN_GL_RENDERER_H
