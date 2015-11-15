#include "Build/ImageCompiler.h"
#include "png.h"

struct RoguePNGDecoder
{
  RogueInteger width;
  RogueInteger height;
  RogueInteger bpp;

  jmp_buf      error_jmp_buffer;
  RogueByte*   png_data_cursor;
  RogueInteger png_data_remaining;
};


static void Plasmacore_png_error_handler( png_structp png_ptr, png_const_charp msg )
{
  fprintf(stderr, "libpng error: %s\n", msg);
  fflush(stderr);

  longjmp( ((RoguePNGDecoder*)png_get_io_ptr(png_ptr))->error_jmp_buffer, 1 );
}

static void Plasmacore_png_read_callback( png_structp png_ptr, png_bytep data, png_size_t count )
{
  RoguePNGDecoder* decoder = (RoguePNGDecoder*) png_get_io_ptr( png_ptr );
  if (count > decoder->png_data_remaining) count = (png_size_t) decoder->png_data_remaining;

  if (count)
  {
    memcpy( data, decoder->png_data_cursor, count );
    decoder->png_data_cursor    += count;
    decoder->png_data_remaining -= count;
  }
}

static bool Plasmacore_decode_png( RoguePNGDecoder* decoder, char* data, int data_size )
{
  png_structp  png_ptr;
  png_infop    info_ptr;

  decoder->png_data_cursor = (RogueByte*) data;
  decoder->png_data_remaining = (RogueInteger) data_size;

  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, decoder,
      Plasmacore_png_error_handler, NULL );
  if ( !png_ptr ) return false; // Out of memory

  png_set_add_alpha( png_ptr, 255, PNG_FILLER_AFTER );

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct( &png_ptr, NULL, NULL );
    return false;  // Out of memory
  }

  if (setjmp(decoder->error_jmp_buffer))
  {
    png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
    return false;
  }

  png_set_read_fn( png_ptr, decoder, Plasmacore_png_read_callback );

#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
  // Prepare the reader to ignore all recognized chunks whose data won't be
  // used, i.e., all chunks recognized by libpng except for IHDR, PLTE, IDAT,
  // IEND, tRNS, bKGD, gAMA, and sRGB (small performance improvement).
  {
    static png_byte chunks_to_ignore[] = 
    {
       99,  72,  82,  77, '\0',  // cHRM
      104,  73,  83,  84, '\0',  // hIST
      105,  67,  67,  80, '\0',  // iCCP
      105,  84,  88, 116, '\0',  // iTXt
      111,  70,  70, 115, '\0',  // oFFs
      112,  67,  65,  76, '\0',  // pCAL
      112,  72,  89, 115, '\0',  // pHYs
      115,  66,  73,  84, '\0',  // sBIT
      115,  67,  65,  76, '\0',  // sCAL
      115,  80,  76,  84, '\0',  // sPLT
      115,  84,  69,  82, '\0',  // sTER
      116,  69,  88, 116, '\0',  // tEXt
      116,  73,  77,  69, '\0',  // tIME
      122,  84,  88, 116, '\0'   // zTXt
    };

    png_set_keep_unknown_chunks( png_ptr, PNG_HANDLE_CHUNK_NEVER,
        chunks_to_ignore, sizeof(chunks_to_ignore)/5 );
  }
#endif // PNG_HANDLE_AS_UNKNOWN_SUPPORTED

  png_read_png(
      png_ptr,
      info_ptr,
      PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_GRAY_TO_RGB,
      NULL
  );

  png_uint_32 width  = png_get_image_width( png_ptr, info_ptr );
  png_uint_32 height = png_get_image_height( png_ptr, info_ptr );

  //Texture_init( NULL, width, height, plasmacore_png_texture_format );

  png_bytepp row_pointers = png_get_rows( png_ptr, info_ptr );

  int row_size = (int) png_get_rowbytes( png_ptr, info_ptr );

  /*
  for (int j=0; j<height; ++j)
  {
    // The appropriate GL texture was prepped just before this PNG decoding
    if (plasmacore_png_texture_format == 1)
    {
      // premultiply the alpha
      int count = width + 1;
      unsigned char* data = row_pointers[j];
      while (--count)
      {
        int a = data[3];
        data[0] = (data[0] * a) / 255;
        data[1] = (data[1] * a) / 255;
        data[2] = (data[2] * a) / 255;
        data += 4;
      }
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, j, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, row_pointers[j] );
    }
    else
    {
      // premultiply the alpha and convert it to 16-bit
      int count = width + 1;
      unsigned char* src = row_pointers[j];
      unsigned short* dest = ((unsigned short*) row_pointers[j]) - 1;
      while (--count)
      {
        int a = src[3];
        int r = ((src[0] * a) / 255) >> 4;
        int g = ((src[1] * a) / 255) >> 4;
        int b = ((src[2] * a) / 255) >> 4;
        a >>= 4;
        *(++dest) = (unsigned short) ((r<<12) | (g<<8) | (b<<4) | a);
        src  += 4;
      }
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, j, width, 1, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, row_pointers[j] );
    }
  }
  */

  png_destroy_read_struct( &png_ptr, &info_ptr, NULL );

  decoder->width = (RogueInteger)  width;
  decoder->height = (RogueInteger) height;

  return true;
}

int main( int argc, const char* argv[] )
{
  Rogue_configure();
  Rogue_launch( argc, argv );
  Rogue_quit();
  return 0;
}
