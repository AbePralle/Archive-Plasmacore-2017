//=============================================================================
//  PlasmacoreStringTable.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef PLASMACORE_STRING_TABLE_H
#define PLASMACORE_STRING_TABLE_H

#include <cstring>

//=============================================================================
//  PlasmacoreStringTableEntry
//=============================================================================
template <class DataType>
struct PlasmacoreStringTableEntry
{
  char*             key;
  int               hash_code;
  DataType          value;
  PlasmacoreStringTableEntry* next_entry;

  PlasmacoreStringTableEntry( const char* key, int hash_code, DataType value )
      : hash_code(hash_code), value(value), next_entry(NULL)
  {
    strcpy( (this->key = new char[std::strlen(key)+1]), key );
  }

  ~PlasmacoreStringTableEntry()
  {
    //printf( "DELETING %s\n", key );
    delete key;
  }
};


//=============================================================================
//  PlasmacoreStringTable
//=============================================================================
template <class DataType>
struct PlasmacoreStringTable
{
  //---------------------------------------------------------------------------
  //  PlasmacoreStringTable::Reader
  //---------------------------------------------------------------------------
  struct Reader
  {
    PlasmacoreStringTable<DataType>*      table;
    PlasmacoreStringTableEntry<DataType>* cur_entry;
    int                         cur_bin_index;
    int                         index;

    Reader( PlasmacoreStringTable<DataType>* table, int starting_index )
      : table(table), cur_entry(NULL), cur_bin_index(-1), index(-1)
    {
      if (starting_index < table->count)
      {
        for (int i=0; i<=starting_index; ++i) advance_entry();
      }
      else
      {
        index = table->count;
      }
    }

    void advance_entry()
    {
      ++index;

      if (cur_entry)
      {
        cur_entry = cur_entry->next_entry;
        if (cur_entry) return;
      }

      while (++cur_bin_index < table->bin_count)
      {
        cur_entry = table->bins[cur_bin_index];
        if (cur_entry) return;
      }

      cur_entry = NULL;
    }

    bool has_another()
    {
      return !!cur_entry;
    }

    bool operator!=( const Reader& other ) const
    {
      return index != other.index;
    }

    PlasmacoreStringTableEntry<DataType>* operator*() const
    {
      return cur_entry;
    }

    PlasmacoreStringTableEntry<DataType>* operator->() const
    {
      return cur_entry;
    }

    const Reader& operator++()
    {
      advance_entry();
      return *this;
    }

    PlasmacoreStringTableEntry<DataType>* read()
    {
      PlasmacoreStringTableEntry<DataType>* result = cur_entry;
      advance_entry();
      return result;
    }
  };

  //---------------------------------------------------------------------------
  //  PlasmacoreStringTable
  //---------------------------------------------------------------------------
  // PROPERTIES
  PlasmacoreStringTableEntry<DataType>** bins;
  int bin_count;
  int bin_mask;
  int count;

  // METHODS
  PlasmacoreStringTable( int initial_bin_count=16 )
    : bins(NULL), count(0)
  {
    int p2 = 1;
    while (p2 < initial_bin_count) p2 <<= 1;
    this->bin_count = initial_bin_count = p2;

    bin_mask = initial_bin_count - 1;

    if (bins)
    {
      clear();
      delete bins;
    }

    bins = new PlasmacoreStringTableEntry<DataType>*[ initial_bin_count ];
    memset( bins, 0, initial_bin_count * sizeof(PlasmacoreStringTableEntry<DataType>*) );
  }


  ~PlasmacoreStringTable()
  {
    if (bins)
    {
      clear();

      delete bins;
      bins = NULL;
      count = 0;
    }
  }

  Reader begin() { return Reader(this,0); }
  Reader end() { return Reader(this,count); }

  int calculate_hash_code( const char* st )
  {
    int code = 0;
    unsigned char ch;
    --st;
    while ((ch = (unsigned char) *(++st))) code = ((code << 3) - code) + ch;  // code = code*7 + ch
    return code;
  }

  void clear()
  {
    while (count) remove_another();
  }

  bool contains( const char* key )
  {
    return find(key) != NULL;
  }

  void expand( int new_bin_count )
  {
    // Internal use.  Increase the number of bins and reinsert the entries.
    if (new_bin_count <= bin_count) return;

    // Ensure the new bin count is a power of 2
    int bc = 1;
    while (bc < new_bin_count) bc <<= 1;
    new_bin_count = bc;

    // Combine all entries into a single linked list
    PlasmacoreStringTableEntry<DataType>* all_entries = 0;
    for (int i=0; i<bin_count; ++i)
    {
      PlasmacoreStringTableEntry<DataType>* cur = bins[ i ];
      while (cur)
      {
        PlasmacoreStringTableEntry<DataType>* next = cur->next_entry;
        cur->next_entry = all_entries;
        all_entries = cur;
        cur = next;
      }
    }

    // Recreate the bins
    delete bins;
    bin_count = new_bin_count;
    bin_mask = bin_count - 1;
    bins = new PlasmacoreStringTableEntry<DataType>*[ bin_count ];
    memset( bins, 0, bin_count * sizeof(PlasmacoreStringTableEntry<DataType>*) );

    // Reindex each entry
    while (all_entries)
    {
      PlasmacoreStringTableEntry<DataType>* cur = all_entries;
      all_entries = all_entries->next_entry;

      int bindex = cur->hash_code & bin_mask;
      cur->next_entry = bins[ bindex ];
      bins[ bindex ] = cur;
    }
  }

  PlasmacoreStringTableEntry<DataType>* find( const char* key )
  {
    return find( key, calculate_hash_code(key) );
  }

  PlasmacoreStringTableEntry<DataType>* find( const char* key, int hash_code )
  {
    PlasmacoreStringTableEntry<DataType>* cur_entry = bins[ hash_code & bin_mask ];

    while (cur_entry)
    {
      if (cur_entry->hash_code==hash_code && (cur_entry->key==key || 0==strcmp(cur_entry->key,key)))
      {
        return cur_entry;
      }
      cur_entry = cur_entry->next_entry;
    }
    return NULL;
  }

  DataType& get( const char* key )
  {
    return get( key, calculate_hash_code(key) );
  }

  DataType& get( const char* key, int hash_code )
  {
    PlasmacoreStringTableEntry<DataType>* cur_entry = bins[ hash_code & bin_mask ];

    while (cur_entry)
    {
      if (cur_entry->hash_code==hash_code && (cur_entry->key==key || 0==strcmp(cur_entry->key,key)))
      {
        return cur_entry->value;
      }
      cur_entry = cur_entry->next_entry;
    }

    cur_entry = new PlasmacoreStringTableEntry<DataType>( key, hash_code, DataType() );
    cur_entry->next_entry = bins[ hash_code & bin_mask ];
    bins[ hash_code & bin_mask ] = cur_entry;
    ++count;

    if (count > (bin_count << 2)) expand( bin_count * 2 );

    return cur_entry->value;
  }

  inline DataType& operator[]( const char* key ) { return get(key); }

  Reader reader() { return begin(); }

  bool remove( const char* key )
  {
    return remove( key, calculate_hash_code(key) );
  }

  bool remove( const char* key, int hash_code )
  {
    PlasmacoreStringTableEntry<DataType>* cur_entry = bins[ hash_code & bin_mask ];

    if ( !cur_entry ) return false;

    if (cur_entry->hash_code==hash_code && (cur_entry->key==key || 0==strcmp(cur_entry->key,key)))
    {
      // Head of list is item to remove
      bins[ hash_code & bin_mask ] = cur_entry->next_entry;
      delete cur_entry;
      --count;
      return true;
    }

    PlasmacoreStringTableEntry<DataType>* next_entry = cur_entry->next_entry;
    while (next_entry)
    {
      if (cur_entry->hash_code==hash_code && (cur_entry->key==key || 0==strcmp(cur_entry->key,key)))
      {
        cur_entry->next_entry = next_entry->next_entry;
        delete next_entry;
        --count;
        return true;
      }
      cur_entry = next_entry;
      next_entry = cur_entry->next_entry;
    }

    return false;
  }

  DataType remove_another()
  {
    if (bins)
    {
      for (int i=bin_mask; i>=0; --i)
      {
        PlasmacoreStringTableEntry<DataType>* cur_entry = bins[i];
        if (cur_entry)
        {
          PlasmacoreStringTableEntry<DataType>* next_entry = cur_entry->next_entry;
          DataType result = cur_entry->value;
          delete cur_entry;
          bins[i] = next_entry;
          --count;
          return result;
        }
      }
    }

    return DataType();
  }
};


#endif // PLASMACORE_STRING_TABLE_H
