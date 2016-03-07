//=============================================================================
//  SuperCPPCString.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_C_STRING_H
#define SUPER_CPP_C_STRING_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  CString
//=============================================================================
struct CString
{
  char* characters;
  int   count;

  CString();
  CString( const char* value );
  CString( const CString& existing );
  ~CString();

  CString& operator=( const char* value );
  CString& operator=( const CString& other );
  inline char operator[]( int index ) { return characters[index]; }
  operator char*();
  operator const char*();
};

} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_C_STRING_H
