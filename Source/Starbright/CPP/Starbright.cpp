//=============================================================================
//  Starbright.cpp
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
#include <cstdio>
#include <cmath>
using namespace std;

extern double rotation_degrees;

namespace PROJECT_WORKSPACE
{
namespace Starbright
{

//=============================================================================
//  Globals
//=============================================================================
Renderer* renderer = 0;


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
  memset( active_texture_ids, 0, MAX_CONCURRENT_TEXTURES * sizeof(int) );
}

Renderer* Renderer::activate()
{
  renderer = this;
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

void Renderer::set_primitive_type( int type )
{
  if (primitive_type != type)
  {
    render();
    primitive_type = type;
  }
}

void Renderer::set_texture_count( int texture_count )
{
  if (active_texture_count != texture_count)
  {
    render();
    active_texture_count = texture_count;
  }
}

void Renderer::set_shader( int shader_id )
{
  if (active_shader_id != shader_id)
  {
    render();
    active_shader_id = shader_id;
  }
}

void Renderer::set_texture( int index, int texture_id )
{
  if ((unsigned int)index < MAX_CONCURRENT_TEXTURES && active_texture_ids[index] != texture_id)
  {
    render();
    active_texture_ids[index] = texture_id;
    if (active_texture_count <= index) set_texture_count( index + 1 );
  }
}

void Renderer::set_transform( double matrix[16] )
{
  for (int i=16; --i>=0; )
  {
    transform[i] = (float) matrix[i];
  }
}

void Renderer::set_transform( float matrix[16] )
{
  memcpy( transform, matrix, 16 * sizeof(float) );
}

}; // namespace Starbright
}; // namespace PROJECT_WORKSPACE

