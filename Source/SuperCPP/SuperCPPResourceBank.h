//=============================================================================
//  SuperCPPList.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef SUPER_CPP_RESOURCE_BANK_H
#define SUPER_CPP_RESOURCE_BANK_H

#ifndef PROJECT_NAMESPACE
  #define PROJECT_NAMESPACE Project
#endif

#include "SuperCPPList.h"

namespace PROJECT_NAMESPACE
{
namespace SuperCPP
{

//=============================================================================
//  ResourceBank 
//=============================================================================
template <class ResourceType>
struct ResourceBank
{
  // PROPERTIES
  List<ResourceType> resources;
  List<int>          active_ids;
  List<int>          available_ids;

  // METHODS
  ResourceBank()
  {
    clear();
  }

  int add( ResourceType resource )
  {
    int index = create_id();
    set( index, resource );
    return index;
  }

  ResourceBank<ResourceType>& clear()
  {
    resources.clear();
    active_ids.clear();
    available_ids.clear();

    resources.add( 0 );  // skip spot 0
    return *this;
  }

  int count()
  {
    return active_ids.count;
  }

  int create_id()
  {
    // INTERNAL USE
    int result;
    if (available_ids.count)
    {
      result = available_ids.remove_last();
    }
    else
    {
      result = resources.count;
      resources.add( 0 );
    }
    active_ids.add( result );
    return result;
  }

  int locate_first()
  {
    if (active_ids.count == 0) return 0;
    return active_ids.data[0];
  }

  int locate_resource( ResourceType resource )
  {
    int       i = active_ids.count;
    ResourceType* data = resources.data;

    while (--i >= 0)
    {
      if (data[i] == resource) return i;
    }

    return -1;
  }

  ResourceType get( int id )
  {
    if (id <= 0 || id >= resources.count) return 0;
    return resources.data[ id ];
  }

  ResourceType remove( int id )
  {
    int      index;
    ResourceType result;

    if (id <= 0 || id >= resources.count) return 0;

    available_ids.add( id );

    result = resources.data[ id ];
    resources.data[ id ] = 0;

    index = active_ids.locate( id );
    if (index >= 0)
    {
      active_ids.data[index] = active_ids.data[ --active_ids.count ];
    }

    return result;
  }

  ResourceType remove_another()
  {
    if (active_ids.count == 0) return 0;

    int id = active_ids.data[0];
    if ( !id ) return 0;

    return remove( id );
  }

  ResourceBank& set( int id, ResourceType resource )
  {
    if (id <= 0 || id >= resources.count) return *this;
    resources.data[ id ] = resource;
    return *this;
  }
};


} // namespace SuperCPP
} // namespace PROJECT_NAMESPACE

using namespace PROJECT_NAMESPACE;

#endif // SUPER_CPP_RESOURCE_BANK_H
