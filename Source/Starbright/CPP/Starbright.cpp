//=============================================================================
//  Starbright.c
//
//  HISTORY
//    2015.12.08 - Created by Abe Pralle.
//
//  See README.md for instructions.
//=============================================================================

#include "Starbright.h"
using namespace SuperCPP;

#include <stdlib.h>
#include <string.h>

namespace PROJECT_WORKSPACE
{
namespace Starbright
{

//=============================================================================
//  Globals
//=============================================================================
Renderer* active_renderer = 0;

//=============================================================================
//  Matrix
//=============================================================================
Matrix::Matrix()
{
}

Matrix::Matrix( const Matrix& other )
{
  memcpy( this, &other, sizeof(Matrix) );
}

Matrix& Matrix::set_identity()
{
  set_zeros();
  at[0]  = 1;
  at[5]  = 1;
  at[10] = 1;
  at[15] = 1;
  return *this;
}

Matrix& Matrix::set_orthographic( int left, int top, int right, int bottom, double near, double far )
{
  double tx = -(right + left) / (right - left);
  double ty = -(top + bottom) / (top - bottom);
  double tz = -(far + near) / (far - near);

  set_identity();
  r1c1 =  2.0 / (right - left);
  r2c2 =  2.0 / (top - bottom);
  r3c3 = -2.0 / (far - near);
  r1c4 = tx;
  r2c4 = ty;
  r3c4 = tz;

  return *this;
}

Matrix& Matrix::set_zeros()
{
  memset( this, 0, sizeof(Matrix) );
  return *this;
}

Matrix Matrix::times( Matrix& other )
{
  Matrix result;
  result.at[0] = at[0]  * other.at[0]
               + at[4]  * other.at[1]
               + at[8]  * other.at[2]
               + at[12] * other.at[3];

  result.at[1] = at[1]  * other.at[0]
               + at[5]  * other.at[1]
               + at[9]  * other.at[2]
               + at[13] * other.at[3];

  result.at[2] = at[2]  * other.at[0]
               + at[6]  * other.at[1]
               + at[10] * other.at[2]
               + at[14] * other.at[3];

  result.at[3] = at[3]  * other.at[0]
               + at[7]  * other.at[1]
               + at[11] * other.at[2]
               + at[15] * other.at[3];

  result.at[4] = at[0]  * other.at[4]
               + at[4]  * other.at[5]
               + at[8]  * other.at[6]
               + at[12] * other.at[7];

  result.at[5] = at[1]  * other.at[4]
               + at[5]  * other.at[5]
               + at[9]  * other.at[6]
               + at[13] * other.at[7];

  result.at[6] = at[2]  * other.at[4]
               + at[6]  * other.at[5]
               + at[10] * other.at[6]
               + at[14] * other.at[7];

  result.at[7] = at[3]  * other.at[4]
               + at[7]  * other.at[5]
               + at[11] * other.at[6]
               + at[15] * other.at[7];

  result.at[8] = at[0]  * other.at[8]
               + at[4]  * other.at[9]
               + at[8]  * other.at[10]
               + at[12] * other.at[11];

  result.at[9] = at[1]  * other.at[8]
               + at[5]  * other.at[9]
               + at[9]  * other.at[10]
               + at[13] * other.at[11];

  result.at[10] = at[2]  * other.at[8]
                + at[6]  * other.at[9]
                + at[10] * other.at[10]
                + at[14] * other.at[11];

  result.at[11] = at[3]  * other.at[8]
                + at[7]  * other.at[9]
                + at[11] * other.at[10]
                + at[15] * other.at[11];

  result.at[12] = at[0]  * other.at[12]
                + at[4]  * other.at[13]
                + at[8]  * other.at[14]
                + at[12] * other.at[15];

  result.at[13] = at[1]  * other.at[12]
                + at[5]  * other.at[13]
                + at[9]  * other.at[14]
                + at[13] * other.at[15];

  result.at[14] = at[2]  * other.at[12]
                + at[6]  * other.at[13]
                + at[10] * other.at[14]
                + at[14] * other.at[15];

  result.at[15] = at[3]  * other.at[12]
                + at[7]  * other.at[13]
                + at[11] * other.at[14]
                + at[15] * other.at[15];

  return result;
}

float* Matrix::to_float( float* dest )
{
  dest[0]  = (float) at[0];
  dest[1]  = (float) at[1];
  dest[2]  = (float) at[2];
  dest[3]  = (float) at[3];
  dest[4]  = (float) at[4];
  dest[5]  = (float) at[5];
  dest[6]  = (float) at[6];
  dest[7]  = (float) at[7];
  dest[8]  = (float) at[8];
  dest[9]  = (float) at[9];
  dest[10] = (float) at[10];
  dest[11] = (float) at[11];
  dest[12] = (float) at[12];
  dest[13] = (float) at[13];
  dest[14] = (float) at[14];
  dest[15] = (float) at[15];
  return dest;
}


//=============================================================================
//  Renderer
//=============================================================================
Renderer::Renderer()
  :
    clear_color(0xff000000),
    clear_depth(1000),
    clear_stencil(0),

    display_width(0),
    display_height(0),
    primitive_type(0),
    render_mode(0),
    active_shader_id(0),
    active_texture_count(0),

    vertex_count(0)
{
}

Renderer* Renderer::activate()
{
  active_renderer = this;
  return this;
}

Vertex* Renderer::add_vertices( int count )
{
  if (vertex_count + count > VERTEX_BUFFER_COUNT)
  {
    render();
  }

  Vertex* result = vertices + vertex_count;
  vertex_count += count;

  return result;
}

void Renderer::set_texture_count( int texture_count )
{
  if (active_texture_count != texture_count)
  {
    render();
    active_texture_count = texture_count;
  }
}

void Renderer::set_transform_2d( double left, double top, double right, double bottom )
{
  render();
  projection_transform.set_orthographic( left, top, right, bottom );
}

void Renderer::use_primitive_type( int type )
{
  if (primitive_type != type)
  {
    render();
    primitive_type = type;
  }
}

void Renderer::use_shader( int shader_id )
{
  if (active_shader_id != shader_id)
  {
    render();
    active_shader_id = shader_id;
  }
}

void Renderer::use_texture( int index, int texture_id )
{
  if ((unsigned int)index < MAX_CONCURRENT_TEXTURES && active_texture_ids[index] != texture_id)
  {
    render();
    active_texture_ids[index] = texture_id;
  }
}

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE

