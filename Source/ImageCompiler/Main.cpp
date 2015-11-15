#include "Build/ImageCompiler.h"

int main( int argc, const char* argv[] )
{
  Rogue_configure();
  Rogue_launch( argc, argv );
  Rogue_quit();
  return 0;
}
