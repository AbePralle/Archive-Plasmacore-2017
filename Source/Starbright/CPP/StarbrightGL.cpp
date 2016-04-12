#include "SuperCPP.h"
#include "StarbrightGL.h"
using namespace SuperCPP;

#include <stdio.h>
#include <string.h>

namespace PROJECT_WORKSPACE
{
namespace Starbright
{

//-----------------------------------------------------------------------------
//  GLRenderer
//-----------------------------------------------------------------------------
GLRenderer::GLRenderer()
  : color_shader(0),
    texture_shader(0),
    texture_shader_with_color_multiply(0),
    premultiplied_texture_shader_with_color_add(0),
    premultiplied_texture_shader_with_color_fill(0)
{
  color_shader = define_shader(
    // Vertex Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform mat4   transform;                 \n"
    "attribute vec4 position;                  \n"
    "attribute" SB_LOWP "vec4 color;           \n"
    "varying  " SB_LOWP "vec4 vertex_color;    \n"
    "void main()                               \n"
    "{                                         \n"
    "  gl_Position = transform * position;     \n"
    "  vertex_color = color / 255.0;           \n"
    "}                                         \n",

    // Pixel Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "varying" SB_LOWP "vec4 vertex_color;    \n"
    "void main()                             \n"
    "{                                       \n"
    "  gl_FragColor = vertex_color;          \n"
    "}                                       \n"
  );

  texture_shader = define_shader(
    // Vertex Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform mat4   transform;             \n"
    "attribute vec4 position;              \n"
    "attribute      vec2 uv;               \n"
    "varying        vec2 vertex_uv;        \n"
    "void main()                           \n"
    "{                                     \n"
    "  gl_Position = transform * position; \n"
    "  vertex_uv = uv;                     \n"
    "}                                     \n",

    // Pixel Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform              sampler2D texture_0;        \n"
    "varying              vec2      vertex_uv;        \n"
    "void main()                                      \n"
    "{                                                \n"
    "  gl_FragColor = texture2D(texture_0,vertex_uv); \n"
    "}                                                \n"
  );

  texture_shader_with_color_multiply = define_shader(
    // Vertex Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform mat4   transform;                  \n"
    "attribute vec4 position;                   \n"
    "attribute      vec2 uv;                    \n"
    "varying        vec2 vertex_uv;             \n"
    "attribute" SB_LOWP "vec4 color;         \n"
    "varying  " SB_LOWP "vec4 vertex_color;  \n"
    "void main()                                \n"
    "{                                          \n"
    "  gl_Position = transform * position;      \n"
    "  vertex_uv = uv;                          \n"
    "  vertex_color = color / 255.0;            \n"
    "}                                          \n",

    // Pixel Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform              sampler2D texture_0;                       \n"
    "varying              vec2      vertex_uv;                       \n"
    "varying" SB_LOWP "vec4 vertex_color;                            \n"
    "void main()                                                     \n"
    "{                                                               \n"
    "  gl_FragColor = texture2D(texture_0,vertex_uv) * vertex_color; \n"
    "}                                                               \n"
  );

  premultiplied_texture_shader_with_color_add = define_shader(
    // Vertex Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform mat4   transform;                 \n"
    "attribute vec4 position;                  \n"
    "attribute      vec2 uv;                   \n"
    "varying        vec2 vertex_uv;            \n"
    "attribute" SB_LOWP "vec4 color;        \n"
    "varying  " SB_LOWP "vec4 vertex_color; \n"
    "void main()                               \n"
    "{                                         \n"
    "  gl_Position = transform * position;     \n"
    "  vertex_uv = uv;                         \n"
    "  vertex_color = color / 255.0;           \n"
    "}                                         \n",

    // Pixel Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform              sampler2D texture_0;                       \n"
    "varying              vec2      vertex_uv;                     \n"
    "varying" SB_LOWP "vec4 vertex_color;                       \n"
    "void main()                                                   \n"
    "{                                                             \n"
    "  vec4 texture_color = texture2D(texture_0,vertex_uv);          \n"
    "  gl_FragColor = vec4( texture_color.xyz + (vertex_color.xyz * texture_color.a), texture_color.a ); \n"
    "}                                                             \n"
  );

  premultiplied_texture_shader_with_color_fill = define_shader(
    // Vertex Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform mat4   transform;                            \n"
    "attribute vec4 position;                             \n"
    "attribute      vec2 uv;                              \n"
    "varying        vec2 vertex_uv;                       \n"
    "attribute" SB_LOWP "vec4  color;                  \n"
    "varying  " SB_LOWP "vec4  vertex_color;           \n"
    "varying  " SB_LOWP "float vertex_inverse_a;       \n"
    "void main()                                          \n"
    "{                                                    \n"
    "  gl_Position = transform * position;                \n"
    "  vertex_uv = uv;                                    \n"
    "  vertex_color = color / 255.0;                      \n"
    "  vertex_inverse_a = 1.0 - vertex_color.a;           \n"
    "}                                                    \n",

    // Pixel Shader
    SB_GLSL_VERSION
    SB_PRECISION_MEDIUMP_FLOAT
    "uniform                sampler2D texture_0;            \n"
    "varying                vec2      vertex_uv;          \n"
    "varying" SB_LOWP   "vec4      vertex_color;       \n"
    "varying  " SB_LOWP "float vertex_inverse_a;       \n"
    "void main()                                          \n"
    "{                                                    \n"
    "  vec4 texture_color = texture2D(texture_0,vertex_uv); \n"
    "  gl_FragColor = vec4( ((texture_color.xyz*vertex_inverse_a)+vertex_color.xyz) * texture_color.a, texture_color.a );  \n"
    "}                                                   \n"
  );

  memset( transform, 0, 16 * sizeof(float) );
  transform[0]  = 1.0f;
  transform[5]  = 1.0f;
  transform[10] = 1.0f;
  transform[15] = 1.0f;
}

GLRenderer::~GLRenderer()
{
  // Delete shaders
  while (shaders.count())
  {
    delete_shader( shaders.locate_first() );
  }

  while (textures.count())
  {
    delete_texture( textures.locate_first() );
  }
}

void GLRenderer::begin_draw( int _display_width, int _display_height )
{
  display_width  = _display_width;
  display_height = _display_height;
}

void GLRenderer::end_draw()
{
  flush();
}

void GLRenderer::clear( int flags )
{
  int gl_flags = 0;
  if (flags & Renderer::CLEAR_COLOR)
  {
    int color = clear_color;
    gl_flags |= GL_COLOR_BUFFER_BIT;
    glClearColor( ((color>>16)&255)/255.0f, ((color>>8)&255)/255.0f, (color&255)/255.0f, ((color>>24)&255)/255.0f );
  }

  if (flags & Renderer::CLEAR_DEPTH)
  {
    double depth = clear_depth;
    gl_flags |= GL_DEPTH_BUFFER_BIT;
    glClearDepth( (float) depth );
  }

  if (flags & Renderer::CLEAR_STENCIL)
  {
    int stencil = clear_stencil;
    gl_flags |= GL_STENCIL_BUFFER_BIT;
    glClearStencil( stencil );
  }

  if (gl_flags) glClear( gl_flags );
}

void GLRenderer::check_for_shader_compile_errors( int shader_id )
{
  GLint success = 0;
  glGetShaderiv( shader_id, GL_COMPILE_STATUS, &success );
  if (success) return;

  GLint log_size = 0;
  glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &log_size );

  char* log = new char[ log_size ];
  glGetShaderInfoLog( shader_id, log_size, &log_size, log );

  printf( "ERROR compiling StarbrightGL shader:\n%s\n", log );
  delete[] log;
}

// COLOR|DEPTH|STENCIL
int  GLRenderer::define_shader( const char* vertex_src, const char* pixel_src )
{
  GLShader* shader = new GLShader();

  shader->vertex_shader = glCreateShader( GL_VERTEX_SHADER );
  shader->pixel_shader = glCreateShader( GL_FRAGMENT_SHADER );

  glShaderSource( shader->vertex_shader, 1, (const char**) &vertex_src, 0 );
  glCompileShader( shader->vertex_shader );
  check_for_shader_compile_errors( shader->vertex_shader );

  glShaderSource( shader->pixel_shader, 1, (const char**) &pixel_src, 0 );
  glCompileShader( shader->pixel_shader );
  check_for_shader_compile_errors( shader->pixel_shader );

  shader->program = glCreateProgram();
  glAttachShader( shader->program, shader->vertex_shader );
  glAttachShader( shader->program, shader->pixel_shader );

  glLinkProgram( shader->program );

  shader->transform_setting  = glGetUniformLocation( shader->program, "transform" );
  shader->position_attribute = glGetAttribLocation( shader->program, "position" );
  shader->color_attribute    = glGetAttribLocation( shader->program, "color" );
  shader->uv_attribute       = glGetAttribLocation( shader->program,  "uv" );

  {
    int i;
    for (i=0; i<Renderer::MAX_CONCURRENT_TEXTURES; ++i)
    {
      char name[80];
      sprintf( name, "texture_%d", i );
      shader->texture_settings[i] = glGetUniformLocation( shader->program, name );
    }
  }

  return shaders.add( shader );
}

int  GLRenderer::define_texture( int texture_id, void* pixels, int width, int height, int options )
{
  GLTexture* texture = (GLTexture*) textures.get( texture_id );
  if ( !texture )
  {
    texture = new GLTexture();
    glGenTextures( 1, &texture->gl_id );
    texture->texture_id = textures.add( texture );
  }

  int bpp = options & (32|16|8);

  texture->width  = width;
  texture->height = height;
  texture->options = options;

  glBindTexture( GL_TEXTURE_2D, texture->gl_id );

  switch (bpp)
  {
    case 32:
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );
      break;

    case 16:
      {
        // Convert 16-bit ARGB to 16-bit RGBA
        int count = width*height;
        UInt16* cursor = ((UInt16*) pixels) - 1;
        while (--count >= 0)
        {
          UInt16 pixel = *(++cursor);
          *cursor = (UInt16)
                  ( ((pixel<<4) & 0x0f00)
                  | ((pixel>>4)&0xf0)
                  | ((pixel<<12)&0xf000)
                  | ((pixel>>12)&15) );
        }
      }
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pixels );
      break;
  }

  return texture->texture_id;
}

void GLRenderer::delete_shader( int shader_id )
{
  GLShader* shader = (GLShader*) shaders.remove( shader_id );
  if ( !shader ) return;

  glDeleteShader( shader->pixel_shader );
  glDeleteShader( shader->pixel_shader );
  glDeleteProgram( shader->program );

  delete shader;
}

void GLRenderer::delete_texture( int texture_id )
{
  GLTexture* texture = (GLTexture*) textures.remove( texture_id );
  if ( !texture ) return;

  glDeleteTextures( 1, &texture->gl_id );

  delete texture;
}

void GLRenderer::flush()
{
  render();
  glFlush();
}

void GLRenderer::render()
{
  GLShader* shader;

  if ( !vertex_count ) return;

  //glDisable( GL_CULL_FACE );

  RenderMode render_mode = this->render_mode;
  if ( !active_shader_id )
  {
    // Choose a built-in shader based on the render mode.
    active_shader_id = color_shader;

    if (render_mode.textures_enabled())
    {
      switch (render_mode.value & RenderMode::VERTEX_COLOR_MODE_MASK)
      {
        case RenderMode::VERTEX_COLOR_MULTIPLY:
          active_shader_id = texture_shader_with_color_multiply;
          break;
        case RenderMode::VERTEX_COLOR_ADD:
          active_shader_id = premultiplied_texture_shader_with_color_add;
          break;
        case RenderMode::VERTEX_COLOR_FILL:
          active_shader_id = premultiplied_texture_shader_with_color_fill;
          break;
        default:
          active_shader_id = texture_shader;
      }
    }
  }

  shader = (GLShader*) shaders.get( active_shader_id );
  if ( !shader ) return;

  glViewport( 0, 0, display_width, display_height );

  if (render_mode.blend_enabled())
  {
    int gl_src_blend, gl_dest_blend;

    switch (render_mode.src_blend())
    {
      case RenderMode::BLEND_ZERO:              gl_src_blend = GL_ZERO;                break;
      case RenderMode::BLEND_ONE:               gl_src_blend = GL_ONE;                 break;
      case RenderMode::BLEND_SRC_ALPHA:         gl_src_blend = GL_SRC_ALPHA;           break;
      case RenderMode::BLEND_INVERSE_SRC_ALPHA: gl_src_blend = GL_ONE_MINUS_SRC_ALPHA; break;
      default:
        printf( "Superbright OpenGL Renderer: Invalid source blend (%d), defaulting to BLEND_ONE.\n", render_mode.src_blend() );
        gl_src_blend = GL_ONE;
    }

    switch (render_mode.dest_blend())
    {
      case RenderMode::BLEND_ZERO:              gl_dest_blend = GL_ZERO;                break;
      case RenderMode::BLEND_ONE:               gl_dest_blend = GL_ONE;                 break;
      case RenderMode::BLEND_SRC_ALPHA:         gl_dest_blend = GL_SRC_ALPHA;           break;
      case RenderMode::BLEND_INVERSE_SRC_ALPHA: gl_dest_blend = GL_ONE_MINUS_SRC_ALPHA; break;
      default:
        printf( "Superbright OpenGL Renderer: Invalid destination blend (%d), defaulting to BLEND_ZERO.\n", render_mode.dest_blend() );
        gl_dest_blend = GL_ONE;
    }

    glEnable( GL_BLEND );
    glBlendFunc( gl_src_blend, gl_dest_blend );
  }
  else
  {
    glDisable( GL_BLEND );
  }

  if (active_texture_count && render_mode.textures_enabled())
  {
    int i;
    for (i=active_texture_count; --i>=0; )
    {
      int texture_id = active_texture_ids[i];
      GLTexture* texture = (GLTexture*) textures.get( texture_id );

      if (texture)
      {
        glActiveTexture( GL_TEXTURE0+i );
        glBindTexture( GL_TEXTURE_2D, texture->gl_id );
        glUniform1i( shader->texture_settings[i], i );
      }

      if (render_mode.wrap_h_enabled())
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
      }
      else
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      }

      if (render_mode.wrap_v_enabled())
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
      }
      else
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      }

      if (render_mode.point_filter_enabled())
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      }
      else
      {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      }
    }
  }

  // ---- Set Vertex Color Mode -----------------------------------------------
  // Copy vertex colors and set up vertex color mode
  Vertex* src_vertex = vertices - 1;
  GLint*    dest_color = color_buffer - 1;
  int count = vertex_count;

  // Swap red and blue AND multiply R,G,B by A.
  while (--count >= 0)
  {
    int color = (++src_vertex)->color;
    int a = (color >> 24) & 255;
    int r = (((color >> 16) & 255) * a) / 255;
    int g = (((color >> 8) & 255) * a) / 255;
    int b = ((color & 255) * a) / 255;
    *(++dest_color) = (a << 24) | (b << 16) | (g << 8) | r;
  }

  // Swap red and blue without premultiplying vertex color
  //while (--count >= 0)
  //{
  //  int color = (++src_vertex)->color;
  //  *(++dest_color) = (color & 0xff00ff00) | ((color >> 16) & 0xff) | ((color << 16) & 0xff0000);
  //}

  if (shader->uv_attribute >= 0)
  {
    // Copy position and uv data at the same time
    Vertex*       src_vertex = vertices - 1;
    GLVertexXYZW* dest_xyzw  = vertex_buffer_xyzw - 1;
    GLVertexUV*   dest_uv    = vertex_buffer_uv - 1;
    int count = vertex_count;
    while (--count >= 0)
    {
      (++dest_xyzw)->x = (++src_vertex)->x;
      dest_xyzw->y     = src_vertex->y;
      dest_xyzw->z     = src_vertex->z;
      dest_xyzw->w     = src_vertex->w;
      (++dest_uv)->u   = src_vertex->u;
      dest_uv->v       = src_vertex->v;
    }
  }
  else
  {
    // Copy position data only
    Vertex*       src_vertex = vertices - 1;
    GLVertexXYZW* dest_xyzw   = vertex_buffer_xyzw - 1;
    int count = vertex_count;
    while (--count >= 0)
    {
      (++dest_xyzw)->x = (++src_vertex)->x;
      dest_xyzw->y     = src_vertex->y;
      dest_xyzw->z     = src_vertex->z;
      dest_xyzw->w     = src_vertex->w;
    }
  }

  // Configure shader
  glUseProgram( shader->program );

  glUniformMatrix4fv( shader->transform_setting, 1, GL_FALSE, transform );
  glVertexAttribPointer( shader->position_attribute, 4, GL_FLOAT, GL_FALSE, 0, vertex_buffer_xyzw );
  glEnableVertexAttribArray( shader->position_attribute );

  if (shader->color_attribute >= 0)
  {
    glVertexAttribPointer( shader->color_attribute, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, color_buffer );
    glEnableVertexAttribArray( shader->color_attribute );
  }

  if (shader->uv_attribute >= 0)
  {
    glVertexAttribPointer( shader->uv_attribute, 2, GL_FLOAT, GL_FALSE, 0, vertex_buffer_uv );
    glEnableVertexAttribArray( shader->uv_attribute );
  }

  switch (primitive_type)
  {
    case Renderer::TRIANGLES:
      glDrawArrays( GL_TRIANGLES, 0, vertex_count );
      break;

    case Renderer::LINES:
      glDrawArrays( GL_LINES, 0, vertex_count );
      break;

    case Renderer::POINTS:
      glDrawArrays( GL_POINTS, 0, vertex_count );
      break;
  }

  // Cleanup
  if (active_texture_count && render_mode.textures_enabled())
  {
    int i;
    for (i=active_texture_count; --i>=0; )
    {
      glActiveTexture( GL_TEXTURE0+i );
    }
  }

  vertex_count = 0;
  memset( vertices, 0, sizeof(Vertex) * Renderer::VERTEX_BUFFER_COUNT );
}

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE
