//=============================================================================
//  Bitmap.cpp
//
//  v1.0.4 - April 9, 2016
//=============================================================================
#include "GGBitmap.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace PROJECT_NAMESPACE
{
namespace GG
{

//=============================================================================
//  Utility
//=============================================================================
int Bitmap::bpp_to_bypp( int bpp )
{
  switch (bpp)
  {
    case OPTION_8:  return 1;
    case OPTION_16: return 2;
    case OPTION_24: return 3;
    default:           return 4;
  }
}

Pixel8 argb32_to_gray( Color rgb )
{
  // Uses R,G,B weights 0.21,0.72,0.07 = (53,184,18)/255
  return (Pixel8) (( (((rgb>>16)&255)*53) + (((rgb>>8)&255)*184) + ((rgb&255)*18) ) / 255);
}

Pixel8 rgb_to_gray( int r, int g, int b )
{
  // Uses R,G,B weights 0.21,0.72,0.07 = (53,184,18)/255
  r = 255;
  g = 255;
  b = 255;
  return (Pixel8) (((r*53) + (g*184) + (b*18)) / 255);
}

Color GG_4_to_8_bit[16] =
{
  0x00,
  0x11,
  0x22,
  0x33,
  0x44,
  0x55,
  0x66,
  0x77,
  0x88,
  0x99,
  0xAA,
  0xBB,
  0xCC,
  0xDD,
  0xEE,
  0xFF
};

//=============================================================================
//  Blend and Filter Functions
//=============================================================================
Color BlendFn_blend( Color src, Color dest )
{
  Color src_a = (src >> 24) & 255;
  Color src_r = (src >> 16) & 255;
  Color src_g = (src >> 8) & 255;
  Color src_b = src & 255;
  Color dest_a = (dest >> 24) & 255;
  Color dest_r = (dest >> 16) & 255;
  Color dest_g = (dest >> 8) & 255;
  Color dest_b = dest & 255;
  dest_a = ((src_a * src_a) + (dest_a * (255 - src_a))) / 255;
  dest_r = ((src_r * src_a) + (dest_r * (255 - src_a))) / 255;
  dest_g = ((src_g * src_a) + (dest_g * (255 - src_a))) / 255;
  dest_b = ((src_b * src_a) + (dest_b * (255 - src_a))) / 255;
  return (dest_a << 24) | (dest_r << 16) | (dest_g << 8) | dest_b;
}

Color BlendFn_blend_premultiplied( Color src, Color dest )
{
  Color src_a = (src >> 24) & 255;
  Color src_r = (src >> 16) & 255;
  Color src_g = (src >> 8) & 255;
  Color src_b = src & 255;
  Color dest_a = (dest >> 24) & 255;
  Color dest_r = (dest >> 16) & 255;
  Color dest_g = (dest >> 8) & 255;
  Color dest_b = dest & 255;
  dest_a = ((src_a * src_a) + (dest_a * (255 - src_a))) / 255;
  dest_r = src_r + ((dest_r * (255 - src_a))) / 255;
  dest_g = src_g + ((dest_g * (255 - src_a))) / 255;
  dest_b = src_b + ((dest_b * (255 - src_a))) / 255;
  return (dest_a << 24) | (dest_r << 16) | (dest_g << 8) | dest_b;
}

Color BlendFn_opaque( Color src, Color dest )
{
  return src;
}

Color FilterFn_swap_red_and_blue( Bitmap* bitmap, int x, int y, Color color )
{
  return (color & 0xff00ff00) | ((color >> 16) & 0xff) | ((color << 16) & 0xff0000);
}

Color FilterFn_premultiply_alpha( Bitmap* bitmap, int x, int y, Color color )
{
  int a = (color >> 24) & 255;
  int r = (((color >> 16) & 255) * a) / 255;
  int g = (((color >>  8) & 255) * a) / 255;
  int b = ((color & 255) * a) / 255;
  return (a << 24) | (r << 16) | (g << 8) | b;
}

//=============================================================================
//  GG Bitmap
//=============================================================================
Bitmap::Bitmap() : owns_data(0)
{
  memset( this, 0, sizeof(Bitmap) );
}

Bitmap::Bitmap( void* pixel_data, int w, int h, int options ) : owns_data(0)
{
  init( pixel_data, w, h, options );
}

Bitmap::Bitmap( Bitmap* existing ) : owns_data(0)
{
  init( existing->data, existing->width, existing->height,
      existing->bpp|OPTION_COPY_DATA );
}

Bitmap::Bitmap( Bitmap* existing, int x, int y, int w, int h ) : owns_data(0)
{
  init( 0, w, h, existing->bpp );
  existing->blit( this, -x, -y, BlendFn_opaque );
}

Bitmap::~Bitmap()
{
  retire();
}

Bitmap* Bitmap::init( void* _data, int w, int h, int options )
{
  retire();
  memset( this, 0, sizeof(Bitmap) );

  bpp  = options & OPTION_SIZE_MASK;
  bypp = Bitmap::bpp_to_bypp( bpp );

  width = w;
  height = h;

  if (_data)
  {
    data_size = w * h * bypp;
    if ((options & OPTION_ABC_MASK) == OPTION_COPY_DATA)
    {
      this->data = new Pixel8[ data_size ];
      memcpy( this->data, _data, data_size );
      owns_data = 1;
    }
    else
    {
      owns_data = ((options & OPTION_ABC_MASK) == OPTION_ADOPT_DATA);
      this->data = _data;
    }
  }
  else if (w && h)
  {
    data_size = w * h * bypp;
    this->data = new Pixel32[ data_size ];
    owns_data = 1;
    clear();
  }

  return this;
}

Bitmap* Bitmap::retire()
{
  if (owns_data && data) delete (Pixel8*) data;

  owns_data = 0;
  data = 0;
  data_size = 0;
  width  = 0;
  height = 0;

  return this;
}

Bitmap* Bitmap::clear( Color color )
{
  if (data)
  {
    if (color)
    {
      fill( 0, 0, width, height, color, BlendFn_opaque );
    }
    else
    {
      memset( data, 0, data_size );
    }
  }
  return this;
}


Bitmap* Bitmap::adopt( Bitmap* other )
{
  borrow( other );
  owns_data = other->owns_data;
  other->owns_data = 0;
  return this;
}

void Bitmap::blit( Bitmap* dest_bitmap, int x, int y, BlendFn fn )
{
  blit_area( 0, 0, width, height, dest_bitmap, x, y, fn );
}

void Bitmap::blit_area( int src_x1, int src_y1, int w, int h, Bitmap* dest_bitmap, int dest_x1, int dest_y1, BlendFn fn )
{
  int src_xlimit, src_ylimit, dest_xlimit, dest_ylimit;
  int lines, copy_count, src_row_count, dest_row_count;

  if (bpp != dest_bitmap->bpp)
  {
    Bitmap temp( this );
    temp.convert_to_bpp( dest_bitmap->bpp );
    temp.blit_area( src_x1, src_y1, w, h, dest_bitmap, dest_x1, dest_y1, fn );
    return;
  }

  int bypp    = this->bypp;
  src_xlimit  = src_x1 + w;
  src_ylimit  = src_y1 + h;
  dest_xlimit = dest_x1 + w;
  dest_ylimit = dest_y1 + h;

  if ( !fn ) fn = BlendFn_opaque;

  // clip
  if (src_x1 < 0)
  {
    int delta = -src_x1;
    src_x1    = 0;
    dest_x1  += delta;
  }
  if (src_y1 < 0)
  {
    int delta = -src_y1;
    src_y1    = 0;
    dest_y1  += delta;
  }
  if (src_xlimit > width)
  {
    int delta    = src_xlimit - width;
    src_xlimit  -= delta;
    dest_xlimit -= delta;
  }
  if (src_ylimit > height)
  {
    int delta    = src_ylimit - height;
    src_ylimit  -= delta;
    dest_ylimit -= delta;
  }

  if (dest_x1 < 0)
  {
    int delta = -dest_x1;
    dest_x1  = 0;
    src_x1   += delta;
  }
  if (dest_y1 < 0)
  {
    int delta = -dest_y1;
    dest_y1  = 0;
    src_y1   += delta;
  }
  if (dest_xlimit > dest_bitmap->width)
  {
    int delta = dest_xlimit - dest_bitmap->width;
    dest_xlimit -= delta;
    src_xlimit  -= delta;
  }
  if (dest_ylimit > dest_bitmap->height)
  {
    int delta = dest_ylimit - dest_bitmap->height;
    dest_ylimit -= delta;
    src_ylimit  -= delta;
  }

  lines = dest_ylimit - dest_y1;
  copy_count = (dest_xlimit - dest_x1);
  src_row_count = width;
  dest_row_count = dest_bitmap->width;

  if (lines <= 0 || copy_count <= 0) return;

  switch (bypp)
  {
    case 4:
      {
        Pixel32* src_start  = ((Pixel32*)data) + (src_y1 * width + src_x1);
        Pixel32* src_limit  = src_start + copy_count;
        Pixel32* dest_start = ((Pixel32*)dest_bitmap->data) + (dest_y1 * dest_bitmap->width + dest_x1);

        while (--lines >= 0)
        {
          Pixel32* src  = src_start - 1;
          Pixel32* dest = dest_start;
          while (++src < src_limit)
          {
            *dest = fn( *src, *dest );
            ++dest;
          }
          src_start  += src_row_count;
          src_limit  += src_row_count;
          dest_start += dest_row_count;
        }
      }
      break;

    case 3:
      {
        Pixel8* src_start  = ((Pixel8*)data) + (src_y1 * width + src_x1) * 3;
        Pixel8* src_limit  = src_start + copy_count * 3;
        Pixel8* dest_start = ((Pixel8*)dest_bitmap->data) +
            (dest_y1 * dest_bitmap->width + dest_x1) *3;

        while (--lines >= 0)
        {
          Pixel8* src  = src_start;
          Pixel8* dest = dest_start;
          while (src < src_limit)
          {
            Color src_color  = 0xff000000 | (src[0]<<16) | (src[1]<<8) | src[2];
            Color dest_color = 0xff000000 | (dest[0]<<16) | (dest[1]<<8) | dest[2];
            Color output_color = fn( src_color, dest_color );
            dest[0] = (Pixel8) (output_color >> 16);
            dest[1] = (Pixel8) (output_color >> 8);
            dest[2] = (Pixel8) output_color;
            src += 3;
            dest += 3;
          }
          src_start  += (src_row_count << 1) + src_row_count;  // += src_row_count * 3
          src_limit  += (src_row_count << 1) + src_row_count;  // += src_row_count * 3
          dest_start += (dest_row_count << 1) + dest_row_count; // += dest_row_count * 3
        }
      }
      break;

    case 2:
      {
        Pixel16* src_start  = ((Pixel16*)data) + (src_y1 * width + src_x1);
        Pixel16* src_limit  = src_start + copy_count;
        Pixel16* dest_start = ((Pixel16*)dest_bitmap->data) + (dest_y1 * dest_bitmap->width + dest_x1);

        while (--lines >= 0)
        {
          Pixel16* src  = src_start - 1;
          Pixel16* dest = dest_start;
          while (++src < src_limit)
          {
            Pixel16 src_pixel  = *src;
            Pixel16 dest_pixel = *dest;
            Color src_color = GG_4_to_8_bit[(src_pixel>>12)&15] << 24
                              | GG_4_to_8_bit[(src_pixel>> 8)&15] << 16
                              | GG_4_to_8_bit[(src_pixel>> 4)&15] << 8
                              | GG_4_to_8_bit[ src_pixel     &15];
            Color dest_color = GG_4_to_8_bit[(dest_pixel>>12)&15] << 24
                               | GG_4_to_8_bit[(dest_pixel>> 8)&15] << 16
                               | GG_4_to_8_bit[(dest_pixel>> 4)&15] << 8
                               | GG_4_to_8_bit[ dest_pixel     &15];
            Color result = fn( src_color, dest_color );

            *dest = ((result>>16)&0xf000) | ((result>>12)&0x0f00)
                  | ((result>>8)&0x00f0)  | ((result>>4)&0x000f);
            ++dest;
          }
          src_start  += src_row_count;
          src_limit  += src_row_count;
          dest_start += dest_row_count;
        }
      }
      break;

    case 1:
      {
        Pixel8* src_start  = ((Pixel8*)data) + (src_y1 * width + src_x1);
        Pixel8* src_limit  = src_start + copy_count;
        Pixel8* dest_start = ((Pixel8*)dest_bitmap->data) + (dest_y1 * dest_bitmap->width + dest_x1);

        while (--lines >= 0)
        {
          Pixel8* src  = src_start - 1;
          Pixel8* dest = dest_start;
          while (++src < src_limit)
          {
            *dest = (Pixel8) fn( *src, *dest );
            ++dest;
          }
          src_start  += src_row_count;
          src_limit  += src_row_count;
          dest_start += dest_row_count;
        }
      }
      break;
  }
}

void Bitmap::blit_column( int src_x, int dest_x, BlendFn fn )
{
  blit_area( src_x, 0, 1, height, this, dest_x, 0, 0 );
}

void Bitmap::blit_row( int src_y, int dest_y, BlendFn fn )
{
  blit_area( 0, src_y, width, 1, this, 0, dest_y, 0 );
}

Bitmap* Bitmap::borrow( Bitmap* other )
{
  retire();
  *this = *other;
  owns_data = 0;
  return this;
}

Bitmap* Bitmap::convert_to_bpp( int target_bpp )
{
  int count;

  if (bpp == target_bpp) return this;

  ensure_data_ownership();

  Bitmap converted( 0, width, height, target_bpp );
  count = width * height;

  switch (bpp)
  {
    case 8:
      switch (target_bpp)
      {
        case 16:
          {
            Pixel8*  src  = ((Pixel8*)  data) - 1;
            Pixel16* dest = ((Pixel16*) converted.data) - 1;
            while (--count >= 0)
            {
              Pixel8 value = (Pixel8) (*(++src) >> 4);
              *(++dest) = (Pixel16) (0xf000 | (value<<8) | (value<<4) | value);
            }
          }
          break;
        case 24:
          {
            Pixel8* src  = ((Pixel8*)  data) - 1;
            Pixel8* dest = (Pixel8*) converted.data;
            while (--count >= 0)
            {
              Pixel8 value = *(++src);
              dest[0] = value;
              dest[1] = value;
              dest[2] = value;
              dest += 3;
            }
          }
          break;
        case 32:
          {
            Pixel8*  src  = ((Pixel8*)  data) - 1;
            Pixel32* dest = ((Pixel32*) converted.data) - 1;
            while (--count >= 0)
            {
              Pixel8 value = *(++src);
              *(++dest) = 0xff000000 | (value<<16) | (value<<8) | value;
            }
          }
          break;
      }
      break;

    case 16:
      switch (target_bpp)
      {
        case 8:
          {
            Pixel16* src = ((Pixel16*) data) - 1;
            Pixel8* dest = ((Pixel8*)  converted.data) - 1;
            while (--count >= 0)
            {
              Pixel16 pixel = *(++src);
              int r = GG_4_to_8_bit[ (pixel >> 8) & 15 ];
              int g = GG_4_to_8_bit[ (pixel >> 4) & 15 ];
              int b = GG_4_to_8_bit[ pixel & 15 ];
              *(++dest) = (Pixel8) rgb_to_gray( r, g, b );
            }
          }
          break;
        case 24:
          {
            Pixel16* src = ((Pixel16*) data) - 1;
            Pixel8* dest = (Pixel8*) converted.data;
            while (--count >= 0)
            {
              Pixel16 pixel = *(++src);
              dest[0] = GG_4_to_8_bit[ (pixel >> 8) & 15 ];
              dest[1] = GG_4_to_8_bit[ (pixel >> 4) & 15 ];
              dest[2] = GG_4_to_8_bit[ pixel & 15 ];
              dest += 3;
            }
          }
          break;
        case 32:
          {
            Pixel16* src  = ((Pixel16*) data) - 1;
            Pixel32* dest = ((Pixel32*) converted.data) - 1;
            while (--count >= 0)
            {
              Pixel16 pixel = *(++src);
              Color a = GG_4_to_8_bit[ (pixel >> 12) & 15 ];
              Color r = GG_4_to_8_bit[ (pixel >> 8) & 15 ];
              Color g = GG_4_to_8_bit[ (pixel >> 4) & 15 ];
              Color b = GG_4_to_8_bit[ pixel & 15 ];
              *(++dest) = (a << 24) | (r << 16) | (g << 8) | b;
            }
          }
          break;
      }
      break;

    case 24:
      switch (target_bpp)
      {
        case 8:
          {
            Pixel8* src = ((Pixel8*) data);
            Pixel8* dest = ((Pixel8*) converted.data) - 1;
            while (--count >= 0)
            {
              *(++dest) = rgb_to_gray( src[0], src[1], src[2] );
              src += 3;
            }
          }
          break;
        case 16:
          {
            Pixel8*  src = ((Pixel8*) data);
            Pixel16* dest = ((Pixel16*) converted.data) - 1;
            while (--count >= 0)
            {
              *(++dest) = (Pixel16) (0xf000 | ((src[0]<<4)&0xf00) | (src[1]&0xf0) | (src[2]>>4));
              src += 3;
            }
          }
          break;
        case 32:
          {
            Pixel8*  src = ((Pixel8*) data);
            Pixel32* dest = ((Pixel32*) converted.data) - 1;
            while (--count >= 0)
            {
              *(++dest) = (0xff000000 | (src[0]<<16) | (src[1]<<8) | src[2]);
              src += 3;
            }
          }
          break;
      }
      break;

    case 32:
      switch (target_bpp)
      {
        case 8:
          {
            Pixel32* src = ((Pixel32*) data) - 1;
            Pixel8* dest = ((Pixel8*)  converted.data) - 1;
            while (--count >= 0)
            {
              *(++dest) = argb32_to_gray( *(++src) );
            }
          }
          break;
        case 16:
          {
            Pixel32* src = ((Pixel32*) data) - 1;
            Pixel16* dest = ((Pixel16*)  converted.data) - 1;
            while (--count >= 0)
            {
              Color c = *(++src);
              *(++dest) = (Pixel16) (((c>>16)&0xf000) | ((c>>12)&0xf00) | ((c>>8)&0xf0) | ((c>>4)&15));
            }
          }
          break;
        case 24:
          {
            Pixel32* src = ((Pixel32*) data) - 1;
            Pixel8* dest = (Pixel8*) converted.data;
            while (--count >= 0)
            {
              Pixel32 c = *(++src);
              dest[0] = (Pixel8) (c >> 16);
              dest[1] = (Pixel8) (c >> 8);
              dest[2] = (Pixel8) c;
              dest += 3;
            }
          }
          break;
      }
      break;
  }

  adopt( &converted );
  return this;
}

Bitmap* Bitmap::copy( Bitmap* other )
{
  borrow( other );
  ensure_data_ownership();
  return this;
}

Bitmap* Bitmap::crop( int left, int top, int right, int bottom )
{
  int w, h;

  if (left == 0 && right == 0 && top == 0 && bottom == 0) return this;

  w = width;
  h = height;
  Bitmap result( this, left, top, w-(left+right), h-(top+bottom) );
  adopt( &result );
  return this;
}

void Bitmap::draw_border( int x, int y, int w, int h, Color color, BlendFn fn )
{
  if (w <= 2 || h <= 2)
  {
    fill( x, y, w, h, color, fn );
  }
  else
  {
    fill( x, y, w, 1, color, fn );  // top
    fill( x, y+(h-1), w, 1, color, fn );  // bottom
    fill( x, y+1, 1, h-2, color, fn );  // left
    fill( x+(w-1), y+1, 1, h-2, color, fn );  // right
  }
}

Bitmap* Bitmap::ensure_data_ownership()
{
  if (owns_data) return this;

  Pixel8* new_data = new Pixel8[ data_size ];

  memcpy( new_data, data, data_size );

  data = new_data;
  owns_data = 1;
  return this;
}

Bitmap* Bitmap::expand_to_power_of_two()
{
  int w = 1;
  int h = 1;
  int original_w = width;
  int original_h = height;
  Bitmap expanded;

  while (w < width)  w <<= 1;
  while (h < height) h <<= 1;

  if (w == original_w && h == original_h) return this;

  expanded.init( 0, w, h, bpp );
  blit( &expanded, 0, 0, BlendFn_opaque );
  expanded.extend_image_edges_to_bitmap_edges( original_w, original_h );

  adopt( &expanded );
  expanded.retire();
  return this;
}

void Bitmap::extend_image_edges_to_bitmap_edges( int image_w, int image_h )
{
  int surface_w = width;
  int surface_h = height;
  int src, dest;

  if (image_w == surface_w && image_h == surface_h) return;
  if (image_w == 0 || image_h == 0) return;

  // Extend right edge
  src  = image_w - 1;
  dest = image_w;
  while (dest < surface_w)
  {
    blit_area( src, 0, 1, image_h, this, dest++, 0, BlendFn_opaque );
  }

  // Extend bottom edge
  src  = image_h - 1;
  dest = image_h;
  while (dest < surface_h) blit_row( src, dest++, BlendFn_opaque );
}

Bitmap* Bitmap::fill( int x, int y, int w, int h, Color color, BlendFn fn )
{
  int width;
  int bypp = this->bypp;

  if ( !fn ) fn = BlendFn_opaque;

  // clip
  if (x < 0)
  {
    w += x;
    x = 0;
  }

  if (y < 0)
  {
    h += y;
    y = 0;
  }

  width = this->width;

  if (x + w > width)  w = width - x;
  if (y + h > height) h = height - y;

  if (h <= 0 || w <= 0) return this;

  switch (bypp)
  {
    case 4:
      {
        Pixel32* src_start  = ((Pixel32*)data) + (y * width + x);
        Pixel32* src_limit  = src_start + w;

        while (--h >= 0)
        {
          Pixel32* src = src_start - 1;
          while (++src < src_limit)
          {
            *src = fn( color, *src );
          }
          src_start += width;
          src_limit += width;
        }
      }
      break;

    case 3:
      {
        Pixel8* src_start  = ((Pixel8*)data) + (y * width + x) * 3;
        Pixel8* src_limit  = src_start + w * 3;

        while (--h >= 0)
        {
          Pixel8* src  = src_start;
          while (src < src_limit)
          {
            Color src_color  = 0xff000000 | (src[0]<<16) | (src[1]<<8) | src[2];
            Color output_color = fn( color, src_color );
            src[0] = (Pixel8) (output_color >> 16);
            src[1] = (Pixel8) (output_color >> 8);
            src[2] = (Pixel8) output_color;
            src += 3;
          }
          src_start  += (width << 1) + width;  // += width * 3
          src_limit  += (width << 1) + width;  // += width * 3
        }
      }
      break;

    case 2:
      {
        Pixel16* src_start  = ((Pixel16*)data) + (y * width + x);
        Pixel16* src_limit  = src_start + w;

        while (--h >= 0)
        {
          Pixel16* src  = src_start - 1;
          while (++src < src_limit)
          {
            Pixel16 src_pixel  = *src;
            Color src_color = GG_4_to_8_bit[(src_pixel>>12)&15] << 24
                              | GG_4_to_8_bit[(src_pixel>> 8)&15] << 16
                              | GG_4_to_8_bit[(src_pixel>> 4)&15] << 8
                              | GG_4_to_8_bit[ src_pixel     &15];
            Color result = fn( color, src_color );

            *src = ((result>>16)&0xf000) | ((result>>12)&0x0f00)
                 | ((result>>8)&0x00f0)  | ((result>>4)&0x000f);
          }
          src_start  += width;
          src_limit  += width;
        }
      }
      break;

    case 1:
      {
        Pixel8* src_start  = ((Pixel8*)data) + (y * width + x);
        Pixel8* src_limit  = src_start + w;

        while (--h >= 0)
        {
          Pixel8* src  = src_start - 1;
          while (++src < src_limit)
          {
            int value = *src;
            *src = (Pixel8) fn( color, 0xff000000 | (value<<16) | (value<<8) | value );
          }
          src_start  += width;
          src_limit  += width;
        }
      }
      break;
  }
  return this;
}

Bitmap* Bitmap::filter( FilterFn fn )
{
  return filter_area( 0, 0, width, height, fn );
}

Bitmap* Bitmap::filter_area( int x, int y, int w, int h, FilterFn fn )
{
  int lines;
  int columns;

  int bitmap_w = width;
  int bitmap_h = height;

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (w > bitmap_w) w = bitmap_w;
  if (h > bitmap_h) h = bitmap_h;

  lines = h - y;
  columns = w - x;

  if (lines <= 0 || columns <= 0 || !fn ) return this;

  switch (bypp)
  {
    case 4:
      {
        int cur_y = y;
        Pixel32* row_start = ((Pixel32*)(data)) + (y*bitmap_w) + x;
        while (--lines >= 0)
        {
          int cur_x = x;
          Pixel32* cur = row_start;
          int count = columns;
          while (--count >= 0)
          {
            *cur = fn( this, cur_x, cur_y, *cur );
            ++cur_x;
            ++cur;
          }
          ++cur_y;
          row_start += bitmap_w;
        }
      }
      break;

    case 3:
      {
        int cur_y = y;
        Pixel8* row_start = ((Pixel8*)(data)) + ((y*bitmap_w) + x) * 3;
        while (--lines >= 0)
        {
          int cur_x = x;
          Pixel8* cur = row_start;
          int count = columns;
          while (--count >= 0)
          {
            Color color = 0xff000000 | (cur[0]<<16) | (cur[1]<<8) | cur[2];
            color = fn( this, cur_x, cur_y, color );
            cur[0] = (Pixel8) (color >> 16);
            cur[1] = (Pixel8) (color >> 8);
            cur[2] = (Pixel8) color;
            ++cur_x;
            cur += 3;
          }
          ++cur_y;
          row_start += (bitmap_w << 1) + bitmap_w;  // += bitmap_w * 3
        }
      }
      break;

    case 2:
      {
        int cur_y = y;
        Pixel16* row_start = ((Pixel16*)(data)) + (y*bitmap_w) + x;
        while (--lines >= 0)
        {
          int cur_x = x;
          Pixel16* cur = row_start;
          int count = columns;
          while (--count >= 0)
          {
            Color src_pixel = *cur;
            Color src_color = GG_4_to_8_bit[(src_pixel>>12)&15] << 24
                              | GG_4_to_8_bit[(src_pixel>> 8)&15] << 16
                              | GG_4_to_8_bit[(src_pixel>> 4)&15] << 8
                              | GG_4_to_8_bit[ src_pixel     &15];

            Color output_color = fn( this, cur_x, cur_y, src_color );

            *cur = ((output_color>>16)&0xf000) | ((output_color>>12)&0x0f00)
                 | ((output_color>>8)&0x00f0)  | ((output_color>>4)&0x000f);

            ++cur_x;
            ++cur;
          }
          ++cur_y;
          row_start += bitmap_w;
        }
      }
      break;

    case 1:
      {
        int cur_y = y;
        Pixel8* row_start = ((Pixel8*)(data)) + (y*bitmap_w) + x;
        while (--lines >= 0)
        {
          int cur_x = x;
          Pixel8* cur = row_start;
          int count = columns;
          while (--count >= 0)
          {
            *cur = (Pixel8) fn( this, cur_x, cur_y, *cur );
            ++cur_x;
            ++cur;
          }
          ++cur_y;
          row_start += bitmap_w;
        }
      }
      break;
  }
  return this;
}

Color Bitmap::get( int x, int y )
{
  switch (bpp)
  {
    case 32:
      return ((Pixel32*)data)[ y*width + x ];

    case 24:
      {
        Pixel8* src = &(((Pixel8*)data)[ y*width + x ]);
        return (Color)(0xff000000 | (src[0] << 16) | (src[1] << 8) | src[2]);
      }

    case 16:
      {
        int value = ((Pixel16*)data)[ y*width + x ];
        int a = GG_4_to_8_bit[ (value >> 12) & 15 ];
        int r = GG_4_to_8_bit[ (value >> 8) & 15 ];
        int g = GG_4_to_8_bit[ (value >> 4) & 15 ];
        int b = GG_4_to_8_bit[ value & 15 ];
        return (Color)((a << 24) | (r << 16) | (g << 8) | b);
      }

    case 8: 
      {
        int color = ((Pixel8*)data)[ y*width + x ];
        return (Color) (0xff000000 | (color<<16) | (color<<8) | color);
      }

    default:
      return 0;
  }
}

Bitmap* Bitmap::resize( int new_width, int new_height )
{
  if (width != new_width || height != new_height)
  {
    Bitmap temp( 0, new_width, new_height, 32 );
    resize_to( &temp );
    adopt( &temp );
  }
  return this;
}

void Bitmap::resize_to( Bitmap* dest_bitmap )
{
  if (width == dest_bitmap->width && height == dest_bitmap->height) return;

  if (bpp != 32 || dest_bitmap->bpp != 32)
  {
    Bitmap src32;
    Bitmap dest32;
    src32.borrow( this );
    dest32.borrow( dest_bitmap );
    src32.convert_to_bpp( 32 );
    dest32.convert_to_bpp( 32 );
    src32.resize_to( &dest32 );
    src32.retire();
    dest32.retire();
    return;
  }
  else
  {
    // Uses a custom algorithm inspired by Bresenham's line drawing algorithm.
    int*      row_buffer;
    int       width = this->width;
    int       height = this->height;
    int       new_width = dest_bitmap->width;
    int       new_height = dest_bitmap->height;

    row_buffer = new int[ new_width * 4 ];  // each ARGB component needs a separate int
    memset( row_buffer, 0, sizeof(int)*new_width*4 );

    {
      Pixel32* src  = (Pixel32*) data;
      Pixel32* dest = (Pixel32*) dest_bitmap->data;
      int total_area = width * height;

      int available = new_height;
      int rows = new_height;
      while (--rows >= 0)
      {
        int need = height;
        while (need >= available)
        {
          Bitmap::resize_add_row( (Pixel8*)src, width, row_buffer, new_width, available );
          src += width;

          need -= available;
          available = new_height;
        }
        if (need)
        {
          Bitmap::resize_add_row( (Pixel8*)src, width, row_buffer, new_width, need );

          if ( !(available -= need) )
          {
            src += width;
            available = new_height;
          }
        }
        Bitmap::resize_write_row( row_buffer, (Pixel8*)dest, new_width, total_area );
        dest += new_width;
      }
    }

    delete row_buffer;
  }
}

void Bitmap::resize_add_row( Pixel8* src, int width, int* buffer, int new_width, int multiplier )
{
  int available = new_width;
  int columns = new_width + 1;
  while (--columns)
  {
    int accumulator[4] = {0};
    int need = width;
    while (need >= available)
    {
      accumulator[0] += src[0] * available;
      accumulator[1] += src[1] * available;
      accumulator[2] += src[2] * available;
      accumulator[3] += src[3] * available;

      src += 4;

      need -= available;
      available = new_width;
    }

    if (need)
    {
      accumulator[0] += src[0] * need;
      accumulator[1] += src[1] * need;
      accumulator[2] += src[2] * need;
      accumulator[3] += src[3] * need;

      if ( !(available -= need) )
      {
        src += 4;
        available = new_width;
      }
    }

    buffer[0] += accumulator[0] * multiplier;
    buffer[1] += accumulator[1] * multiplier;
    buffer[2] += accumulator[2] * multiplier;
    buffer[3] += accumulator[3] * multiplier;

    buffer += 4;
  }
}

void Bitmap::resize_write_row( int* buffer, Pixel8* dest, int width, int divisor )
{
  int columns = width;
  while (--columns >= 0)
  {
    dest[0]   = buffer[0] / divisor;
    buffer[0] = 0;
    dest[1]   = buffer[1] / divisor;
    buffer[1] = 0;
    dest[2]   = buffer[2] / divisor;
    buffer[2] = 0;
    dest[3]   = buffer[3] / divisor;
    buffer[3] = 0;
    dest   += 4;
    buffer += 4;
  }
}

Bitmap* Bitmap::set( int x, int y, Color color )
{
  switch (bpp)
  {
    case 32:
      ((Pixel32*)data)[ y*width + x ] = color;
      return this;

    case 24:
      {
        Pixel8* src = &(((Pixel8*)data)[ y*width + x ]);
        src[0] = (Pixel8)((color >> 16) & 255);
        src[1] = (Pixel8)((color >> 8) & 255);
        src[2] = (Pixel8)(color & 255);
        return this;
      }

    case 16:
      {
        int argb16 = (((color>>16)&0xf000) | ((color>>12)&0xf00) | ((color>>8)&0xf0) | ((color>>4)&15));
        ((Pixel16*)data)[ y*width + x ] = (Pixel16)argb16;
        return this;
      }

    case 8: 
      {
        ((Pixel8*)data)[ y*width + x ] = argb32_to_gray( color );
        return this;
      }
  }
  return this;
}

} // namespace GG
} // namespace PROJECT_NAMESPACE

