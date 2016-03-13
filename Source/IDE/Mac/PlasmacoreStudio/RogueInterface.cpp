#include "RogueInterface.h"

#include <cstdio>
using namespace std;

extern "C" void hello_c( const char* message )
{
  printf( ">> %s\n", message );
}

