//=============================================================================
//  SuperCPP.cpp
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================

#include "SuperCPP.h"

#include <fcntl.h>
#include <math.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#if !defined(CPP_PLATFORM_WINDOWS)
  #include <sys/time.h>
  #include <unistd.h>
  #include <signal.h>
  #include <dirent.h>
  #include <sys/socket.h>
  #include <sys/uio.h>
  #include <sys/stat.h>
  #include <netdb.h>
  #include <errno.h>
  #include <pthread.h>
#endif

#if defined(CPP_PLATFORM_ANDROID)
  #include <netinet/in.h>
#endif

#if defined(CPP_PLATFORM_WINDOWS)
  #include <direct.h>
  #define chdir _chdir
#endif

#if CPP_PLATFORM_IOS 
  #include <sys/types.h>
  #include <sys/sysctl.h>
#endif

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{


//=============================================================================
//  Utility 
//=============================================================================
double current_time_seconds()
{
#if defined(_WIN32)
  struct __timeb64 time_struct;
  double time_seconds;
  _ftime64_s( &time_struct );
  time_seconds = (double) time_struct.time;
  time_seconds += time_struct.millitm / 1000.0;
  return time_seconds;
#elif defined(RVL)
  double time_seconds = OSTicksToMilliseconds(OSGetTime()) / 1000.0;
  return time_seconds;

#else
  struct timeval time_struct;
  double time_seconds;
  gettimeofday( &time_struct, 0 );
  time_seconds = (double) time_struct.tv_sec;
  time_seconds += (time_struct.tv_usec / 1000000.0);
  return time_seconds;
#endif
}

int decoded_utf8_count( const char* utf8_data, int utf8_count )
{
  // Returns the number of 16-bit Characters required to hold the decoded value
  // of the given UTF-8 string.
  if (utf8_count == -1) utf8_count = (int) strlen( utf8_data );

  const char* cur   = utf8_data - 1;
  const char* limit = utf8_data + utf8_count;

  int result_count = 0;
  while (++cur < limit)
  {
    ++result_count;
    int ch = *((unsigned char*)cur);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)      ++cur;
      else if ((ch & 0xf0) == 0xe0) cur += 2;
      else                          { cur += 3; ++result_count; }
    }
  }

  return result_count;
}

int decoded_utf8_count( Character* utf8_data, int utf8_count )
{
  // Returns the number of 16-bit Characters required to hold the decoded value
  // of the given UTF-8 string.
  Character* cur   = utf8_data - 1;
  Character* limit = utf8_data + utf8_count;

  int result_count = 0;
  while (++cur < limit)
  {
    ++result_count;
    int ch = *cur;
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)      ++cur;
      else if ((ch & 0xf0) == 0xe0) cur += 2;
      else                          { cur += 3; ++result_count; }
    }
  }

  return result_count;
}

void decode_utf8( const char* utf8_data, int utf8_count, Character* dest_buffer )
{
  Byte*      src  = (Byte*)(utf8_data - 1);
  Character* dest = dest_buffer - 1;

  if (utf8_count == -1) utf8_count = (int) strlen( utf8_data );

  int remaining_count = utf8_count;
  while (--remaining_count >= 0)
  {
    int ch = *(++src);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)
      {
        // 110x xxxx  10xx xxxx
        if (remaining_count)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
          --remaining_count;
        }
      }
      else if ((ch & 0xf0) == 0xe0)
      {
        // 1110 xxxx  10xx xxxx  10xx xxxx
        if (remaining_count >= 2)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
          ch = (ch << 6) | (*(++src) & 0x3f);
          remaining_count -= 2;
        }
      }
      else
      {
        if (remaining_count >= 3)
        {
          ch = ((ch & 7) << 18) | ((*(++src) & 0x3f) << 12);
          ch |= (*(++src) & 0x3f) << 6;
          ch |= (*(++src) & 0x3f);
          remaining_count -= 3;

          if (ch >= 0x10000)
          {
            // surrogate pair
            ch -= 0x10000;
            *(++dest) = (Character)(0xd800 + ((ch >> 10) & 0x3ff));
            *(++dest) = (Character)(0xdc00 + (ch & 0x3ff));
            continue;
          }
          // else it's just a regular Unicode character
        }
      }
    }
    *(++dest) = (Character) ch;
  }
}

void decode_utf8( Character* utf8_data, int utf8_count, Character* dest_buffer )
{
  Character* src  = (utf8_data - 1);
  Character* dest = dest_buffer - 1;

  int remaining_count = utf8_count;
  while (--remaining_count >= 0)
  {
    int ch = *(++src);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)
      {
        // 110x xxxx  10xx xxxx
        if (remaining_count)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
          --remaining_count;
        }
      }
      else if ((ch & 0xf0) == 0xe0)
      {
        // 1110 xxxx  10xx xxxx  10xx xxxx
        if (remaining_count >= 2)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
          ch = (ch << 6) | (*(++src) & 0x3f);
          remaining_count -= 2;
        }
      }
      else
      {
        if (remaining_count >= 3)
        {
          ch = ((ch & 7) << 18) | ((*(++src) & 0x3f) << 12);
          ch |= (*(++src) & 0x3f) << 6;
          ch |= (*(++src) & 0x3f);
          remaining_count -= 3;

          if (ch >= 0x10000)
          {
            // surrogate pair
            ch -= 0x10000;
            *(++dest) = (Character)(0xd800 + ((ch >> 10) & 0x3ff));
            *(++dest) = (Character)(0xdc00 + (ch & 0x3ff));
            continue;
          }
          // else it's just a regular Unicode character
        }
      }
    }
    *(++dest) = (Character) ch;
  }
}


void double_to_c_string( double value, char* buffer, int size )
{
  // Suggested buffer size: 512
  if (isnan(value))
  {
    snprintf( buffer, size, "NaN" );
  }
  else if (value == INFINITY)
  {
    snprintf( buffer, size, "+infinity" );
  }
  else if (value == -INFINITY)
  {
    snprintf( buffer, size, "-infinity" );
  }
  else if (floor(value) == value)
  {
    if (value > -1e17 and value < 1e17)
    {
      // Whole number with 16 or fewer digits of precision
      snprintf( buffer, size, "%.0lf", value );
    }
    else
    {
      // Whole number with up to 17+ digits of precision, general format.
      snprintf( buffer, size, "%.16lg", value );
    }
  }
  else if (value > -0.1 and value < 0.1)
  {
    // +/- 0.0xxx - use the long general format (g) specifying 17 digits of
    // decimal precision which will result in either a standard decimal value
    // (%lf) or a value in exponent notation (%le), each with UP TO 17 digits
    // of precision (it may be fewer with %g!).
    snprintf( buffer, size, "%.17lg", value );
  }
  else
  {
    // +/- x.xxxx
    snprintf( buffer, size, "%.16lf", value );

    // Round off
    int len = (int) strlen( buffer );
    if (buffer[len-1] < '5')
    {
      buffer[--len] = 0;
    }
    else
    {
      bool carry = true;
      buffer[--len] = 0;
      int i = len - 1;
      while (i >= 0)
      {
        char ch = buffer[i];
        if (ch >= '0' && ch <= '9')
        {
          if (ch == '9')
          {
            buffer[i] = '0';
          }
          else
          {
            ++buffer[i];
            carry = false;
            break;
          }
        }
        --i;
      }
      if (carry)
      {
        // Need one more '1' at the front of all this
        i = -1;
        for (;;)
        {
          char ch = buffer[++i];
          if (ch >= '0' && ch <= '9') break;
        }
        memmove( buffer+i+1, buffer+i, len );
        ++len;
        buffer[i] = '1';
      }
    }

    // rescan the rounded-off value
    sscanf( buffer, "%lf", &value );

    // Remove excess digits as long as the reparsed value doesn't change.
    // Without doing this step 3.1415 would be printed as
    // 3.14150000000000018, for instance.
    //
    // With this process we see:
    //   3.14150000000000018 == 3.1415?  TRUE
    //   3.1415000000000001  == 3.1415?  TRUE
    //   3.141500000000000   == 3.1415?  TRUE
    //   ...
    //   3.1415              == 3.1415?  TRUE
    //   3.141               == 3.1415?  FALSE - add the 5 back on and done
    char temp = 0;
    while (len > 0)
    {
      temp = buffer[--len];  // store new temp
      buffer[len] = 0;
      double n;
      sscanf( buffer, "%lf", &n );
      if (n != value) break;
    }

    // Put back the last character whose absence changed the value.
    buffer[len] = temp;
  }
}

int encoded_utf8_count( Character* characters, int count )
{
  int encoded_count = 0;
  --characters;
  while (--count >= 0)
  {
    Character ch = *(++characters);
    if (ch <= 0x7f)       ++encoded_count;
    else if (ch <= 0x7ff) encoded_count += 2;
    else if (ch >= 0xd800 && ch <= 0xdbff && count > 0)
    {
      // Surrogate pair
      encoded_count += 4;
      --count;
      ++characters;
    }
    else                  encoded_count += 3;
  }
  return encoded_count;
}

void encode_utf8( Character* characters, int count, char* dest_buffer )
{
  --dest_buffer;
  --characters;
  while (--count >= 0)
  {
    Character ch = *(++characters);
    if (ch <= 0x7f)
    {
      // 0xxx xxxx
      *(++dest_buffer) = (char) ch;
    }
    else if (ch <= 0x7ff)
    {
      // 110x xxxx  10xx xxxx
      *(++dest_buffer) = (char) (0xC0 | ((ch >> 6) & 0x1F));
      *(++dest_buffer) = (char) (0x80 | (ch & 0x3F));
    }
    else if (ch >= 0xd800 && ch <= 0xdbff && count > 0)
    {
      // Start of a surrogate pair, decode surrogate pair and then
      // encode that 21-bit value as 4-byte UTF8.
      //
      // 11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
      int low = *(++characters);
      --count;
      if (low >= 0xdc00 && low <= 0xdfff)
      {
        int value = 0x10000 + (((ch - 0xd800)<<10) | (low-0xdc00));
        dest_buffer[1] = (char)(0xf0 | ((value>>18) & 7));
        dest_buffer[2] = (char)(0x80 | ((value>>12) & 0x3f));
        dest_buffer[3] = (char)(0x80 | ((value>>6)  & 0x3f));
        dest_buffer[4] = (char)((value & 0x3f) | 0x80);
        dest_buffer += 4;
      }
    }
    else
    {
      // 1110 xxxx  10xx xxxx  10xx xxxx
      *(++dest_buffer) = (char) (0xE0 | ((ch >> 12) & 0xF));
      *(++dest_buffer) = (char) (0x80 | ((ch >> 6) & 0x3F));
      *(++dest_buffer) = (char) (0x80 | (ch & 0x3F));
    }
  }
}

void encode_utf8( Character* characters, int count, Character* dest_buffer )
{
  --dest_buffer;
  --characters;
  while (--count >= 0)
  {
    Character ch = *(++characters);
    if (ch <= 0x7f)
    {
      // 0xxx xxxx
      *(++dest_buffer) = (char) ch;
    }
    else if (ch <= 0x7ff)
    {
      // 110x xxxx  10xx xxxx
      *(++dest_buffer) = (char) (0xC0 | ((ch >> 6) & 0x1F));
      *(++dest_buffer) = (char) (0x80 | (ch & 0x3F));
    }
    else if (ch >= 0xd800 && ch <= 0xdbff && count > 0)
    {
      // Start of a surrogate pair, decode surrogate pair and then
      // encode that 21-bit value as 4-byte UTF8.
      //
      // 11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
      int low = *(++characters);
      --count;
      if (low >= 0xdc00 && low <= 0xdfff)
      {
        int value = 0x10000 + (((ch - 0xd800)<<10) | (low-0xdc00));
        dest_buffer[0] = (char)(0xf0 | ((value>>18) & 7));
        dest_buffer[1] = (char)(0x80 | ((value>>12) & 0x3f));
        dest_buffer[2] = (char)(0x80 | ((value>>6)  & 0x3f));
        dest_buffer[3] = (char)((value & 0x3f) | 0x80);
        dest_buffer += 4;
      }
    }
    else
    {
      // 1110 xxxx  10xx xxxx  10xx xxxx
      *(++dest_buffer) = (char) (0xE0 | ((ch >> 12) & 0xF));
      *(++dest_buffer) = (char) (0x80 | ((ch >> 6) & 0x3F));
      *(++dest_buffer) = (char) (0x80 | (ch & 0x3F));
    }
  }
}

}; // namespace SuperCPP
}; // namespace PROJECT_NAMESPACE

