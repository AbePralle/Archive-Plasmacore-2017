//=============================================================================
//  Bitmap.h
//
//  v1.0.3 - February 12, 2016
//=============================================================================

#ifndef GG_BITMAP_H
#define GG_BITMAP_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
  #define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdint>

namespace PROJECT_NAMESPACE
{
namespace GG
{

typedef int Color;

#if defined(_WIN32)
  typedef unsigned __int32   Pixel32;
  typedef unsigned __int16   Pixel16;
  typedef unsigned __int8    Pixel8;
#else
  typedef uint32_t           Pixel32;
  typedef uint16_t           Pixel16;
  typedef uint8_t            Pixel8;
#endif

struct Bitmap;

//=============================================================================
//  Blend and Filter Functions
//=============================================================================
typedef Color (*BlendFn)( Color src, Color dest );
typedef Color (*FilterFn)( Bitmap* bitmap, int x, int y, Color color );

Color BlendFn_blend( Color src, Color dest );
Color BlendFn_blend_premultiplied( Color src, Color dest );
Color BlendFn_opaque( Color src, Color dest );
Color FilterFn_swap_red_and_blue( Bitmap* bitmap, int x, int y, Color color );
Color FilterFn_premultiply_alpha( Bitmap* bitmap, int x, int y, Color color );


//=============================================================================
//  Bitmap
//=============================================================================
struct Bitmap
{
  // ADOPT:  Bitmap uses existing data and is responsible for its release.
  // BORROW: Bitmap uses existing data and is not responsible for its release.
  // COPY:   Bitmap copies existing data and is responsible for freeing the copy but not the original.
  static const int OPTION_ADOPT_DATA  = 1;
  static const int OPTION_BORROW_DATA = 0;
  static const int OPTION_COPY_DATA   = 2;
  static const int OPTION_ABC_MASK    = 3;
  static const int OPTION_8           = 8;
  static const int OPTION_16          = 16;
  static const int OPTION_24          = 24;
  static const int OPTION_32          = 32;
  static const int OPTION_SIZE_MASK   = 0x38;

  int   width;      // The width of the bitmap in pixels
  int   height;     // The height of the bitmap in pixels

  int   bpp;        // Bits Per Pixel - 8, 16, 24, or 32
  int   bypp;       // BYtes Per Pixel - 1, 2, or 4

  int   owns_data;  // The 'data should be deleted on a bitmap delete or retire.

  void* data;       // A pointer to 8, 16, 24, or 32-bit pixel data.
  int   data_size;  // The size, in bytes, of the pixel data.

  // METHODS
  Bitmap();
  Bitmap( void* pixel_data, int w, int h, int options );
  Bitmap( Bitmap* existing );
  Bitmap( Bitmap* existing, int x, int y, int w, int h );

  ~Bitmap();

  Bitmap* init( void* data, int w, int h, int options );
  Bitmap* retire();

  Bitmap* adopt( Bitmap* other );
  void    blit( Bitmap* dest_bitmap, int x, int y, BlendFn fn=BlendFn_opaque );
  void    blit_area( int sx, int sy, int w, int h, Bitmap* dest_bitmap, int dx, int dy, BlendFn fn=BlendFn_opaque );
  void    blit_column( int src_x, int dest_x, BlendFn fn=BlendFn_opaque );
  void    blit_row( int src_y, int dest_y, BlendFn fn=BlendFn_opaque );
  Bitmap* borrow( Bitmap* other );
  Bitmap* clear( Color color=0 );
  Bitmap* convert_to_bpp( int bpp );
  Bitmap* copy( Bitmap* other );
  Bitmap* crop( int left, int top, int right, int bottom );
  void    draw_border( int x, int y, int w, int h, Color color, BlendFn fn=BlendFn_opaque );
  Bitmap* ensure_data_ownership();
  Bitmap* expand_to_power_of_two();
  void    extend_image_edges_to_bitmap_edges( int image_w, int image_h );
  Bitmap* fill( int x, int y, int w, int h, Color color, BlendFn fn=BlendFn_opaque );
  Bitmap* filter( FilterFn fn );
  Bitmap* filter_area( int x, int y, int w, int h, FilterFn fn );
  Color   get( int x, int y );
  Bitmap* resize( int new_width, int new_height );
  void    resize_to( Bitmap* dest_bitmap );
  Bitmap* set( int x, int y, Color color );

  static int  bpp_to_bypp( int bpp );
  static void resize_add_row( Pixel8* src, int width, int* buffer, int new_width, int multiplier );
  static void resize_write_row( int* buffer, Pixel8* dest, int width, int divisor );
};


//=============================================================================
//  Utility
//=============================================================================
Pixel8 argb32_to_gray( Color rgb );
Pixel8 rgb_to_gray( int r, int g, int b );

} // namespace GG
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // GG_BITMAP_H
