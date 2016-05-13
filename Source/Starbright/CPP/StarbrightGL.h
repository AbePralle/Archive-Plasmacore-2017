#ifndef STARBRIGHT_GL_H
#define STARBRIGHT_GL_H
//=============================================================================
//  StarbrightGL.h
//
//  HISTORY
//    2015.12.08 - Created by Abe Pralle.
//
//  See README.md for instructions.
//=============================================================================

#include "Starbright.h"

#if CPP_PLATFORM_MAC
  #include <OpenGL/gl.h>

#elif CPP_PLATFORM_IOS
  #include <OpenGLES/ES2/gl.h>
  #include <OpenGLES/ES2/glext.h>
  #define glClearDepth glClearDepthf
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
//  GLShader
//-----------------------------------------------------------------------------
struct GLShader : Shader
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

  GLShader()
  {
    memset( this, 0, sizeof(GLShader) );
  }
};



//-----------------------------------------------------------------------------
//  GLVertexXYZW
//-----------------------------------------------------------------------------
typedef struct GLVertexXYZW
{
  GLfloat x, y, z, w;
} GLVertexXYZW;


//-----------------------------------------------------------------------------
//  GLVertexUV
//-----------------------------------------------------------------------------
typedef struct GLVertexUV
{
  GLfloat u, v;
} GLVertexUV;


//-----------------------------------------------------------------------------
//  GLTexture
//-----------------------------------------------------------------------------
struct GLTexture : Texture
{
  GLuint gl_id;
  int    texture_id;
  int    width, height;
  int    options;

  GLTexture()
  {
    memset( this, 0, sizeof(GLTexture) );
  }
};

//-----------------------------------------------------------------------------
//  GLRenderer
//-----------------------------------------------------------------------------
struct GLRenderer : Renderer
{
  int color_shader;
  int texture_shader;
  int texture_shader_with_color_multiply;
  int premultiplied_texture_shader_with_color_add;
  int premultiplied_texture_shader_with_color_fill;

  GLVertexXYZW vertex_buffer_xyzw[Renderer::VERTEX_BUFFER_COUNT];
  GLVertexUV   vertex_buffer_uv[Renderer::VERTEX_BUFFER_COUNT];
  GLint        color_buffer[Renderer::VERTEX_BUFFER_COUNT];

  GLRenderer();
  ~GLRenderer();

  void begin_draw( int display_width, int display_height );
  void end_draw();
  void clear( int flags ); // COLOR|DEPTH|STENCIL
  void check_for_shader_compile_errors( int shader_id );
  int  define_shader( const char* vertex_src, const char* pixel_src );
  int  define_texture( int texture_id, void* pixels, int width, int height, int options );
  void delete_shader( int shader_id );
  void delete_texture( int texture_id );
  void flush();
  void render();
};

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE
using namespace PROJECT_WORKSPACE;

#endif // STARBRIGHT_GL_H
