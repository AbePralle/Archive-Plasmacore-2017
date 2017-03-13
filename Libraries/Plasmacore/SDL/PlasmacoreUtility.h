//=============================================================================
//  PlasmacoreUtility.h
//=============================================================================
#ifndef PLASMACORE_UTILITY_H
#define PLASMACORE_UTILITY_H

#include <cstring>

//=============================================================================
//  PlasmacoreList
//=============================================================================
template <class DataType>
struct PlasmacoreList
{
  // PROPERTIES
  DataType* data;
  int       count;
  int       capacity;

  // METHODS
  PlasmacoreList( int initial_capacity=0 ) : data(0), count(0), capacity(0)
  {
    reserve( initial_capacity );
  }

  ~PlasmacoreList()
  {
    if (data) delete data;
    capacity = count = 0;
  }

  PlasmacoreList<DataType>& add( DataType value )
  {
    reserve( 1 );
    data[ count++ ] = value;
    return *this;
  }

  PlasmacoreList<DataType>& add( DataType* array, int array_count )
  {
    reserve( array_count );
    std::memcpy( data + count, array, array_count*sizeof(DataType) );
    count += array_count;
    return *this;
  }

  PlasmacoreList<DataType>& add( PlasmacoreList<DataType>& other )
  {
    int n = other.count;
    reserve( n );
    if (n)
    {
      std::memcpy( data, other.data, n*sizeof(DataType) );
    }
    count += n;
    return *this;
  }

  PlasmacoreList<DataType>& adopt( DataType* new_data, int new_count )
  {
    if (data) delete data;
    data = new_data;
    count = new_count;
    capacity = new_count;
  }

  PlasmacoreList<DataType>& clear()
  {
    count = 0;
    return *this;
  }

  DataType first() { return data[0]; }

  PlasmacoreList<DataType>& insert( DataType value, int before_index )
  {
    if (before_index >= count)
    {
      add( value );
      return *this;
    }

    if (before_index < 0) before_index = 0;

    reserve( 1 );

    memmove(
        data + before_index + 1,
        data + before_index,
        (count - before_index) * sizeof(DataType)
      );
    data[ before_index ] = value;
    ++count;

    return *this;
  }

  DataType last() { return data[count-1]; }

  int locate( DataType value )
  {
    int  count = this->count;
    int* data = this->data;

    for (int i=0; i<count; ++i)
    {
      if (data[i] == value) return i;
    }

    return -1;
  }

  DataType& operator[]( int index )
  {
    return data[ index ];
  }

  DataType& operator=( const DataType& other )
  {
    return clear().add( other );
  }

  DataType remove_at( int index )
  {
    if (index == count - 1)
    {
      return remove_last();
    }
    else
    {
      DataType result = data[ index ];
      memmove( data + index, data + index + 1, (--count - index) * sizeof(DataType) );
      return result;
    }
  }

  DataType remove_first()
  {
    return remove_at( 0 );
  }

  DataType remove_last()
  {
    return data[ --count ];
  }

  PlasmacoreList<DataType>& reserve( int additional_capacity )
  {
    int required_capacity = count + additional_capacity;
    if (capacity >= required_capacity) return *this;

    if (data)
    {
      int double_capacity = count << 1;
      if (double_capacity > required_capacity) required_capacity = double_capacity;

      DataType* new_data = new DataType[ required_capacity ];
      std::memcpy( new_data, data, count * sizeof(DataType) );
      delete data;
      data = new_data;
    }
    else
    {
      if (required_capacity < 10) required_capacity = 10;
      data = new DataType[ required_capacity ];
    }

    capacity = required_capacity;
    return *this;
  }

  PlasmacoreList<DataType>& set( int index, DataType value )
  {
    reserve( (index+1) - count );
    if (count <= index) count = index + 1;

    data[ index ] = value;
    return *this;
  }

  DataType* to_array( DataType* array=0, int array_count=-1 )
  {
    int copy_count = count;
    if (array)
    {
      if (array_count >= 0 && array_count < copy_count) copy_count = array_count;
    }
    else
    {
      array = new DataType[ count ];
    }
    std::memcpy( array, data, copy_count * sizeof(DataType) );
    return array;
  }
};


//=============================================================================
//  PlasmacoreCString
//=============================================================================
struct PlasmacoreCString
{
  char* characters;
  int   count;

  PlasmacoreCString();
  PlasmacoreCString( const char* value );
  PlasmacoreCString( const PlasmacoreCString& existing );
  ~PlasmacoreCString();

  PlasmacoreCString& operator=( const char* value );
  PlasmacoreCString& operator=( const PlasmacoreCString& other );
  inline char operator[]( int index ) { return characters[index]; }
  operator char*();
  operator const char*();
  bool operator==( const PlasmacoreCString& other );
  bool operator==( const char* st );
};


//=============================================================================
//  PlasmacoreCStringBuilder
//=============================================================================
struct PlasmacoreCStringBuilder
{
  // PROPERTIES
  char* data;
  int   count;
  int   capacity;
  char  internal_buffer[ 1024 ];

  // METHODS
  PlasmacoreCStringBuilder();
  ~PlasmacoreCStringBuilder();

  PlasmacoreCStringBuilder& add( char value );
  char* as_c_string();
  PlasmacoreCStringBuilder& clear();
  char& operator[]( int index );
  PlasmacoreCStringBuilder& reserve( int additional_capacity );
};


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


//=============================================================================
//  PlasmacoreIntTableEntry
//=============================================================================
template <class DataType>
struct PlasmacoreIntTableEntry
{
  int            key;
  DataType       value;
  PlasmacoreIntTableEntry* next_entry;

  PlasmacoreIntTableEntry( int key, DataType value )
      : key(key), value(value), next_entry(NULL)
  {
  }
};


//=============================================================================
//  PlasmacoreIntTable
//=============================================================================
template <class DataType>
struct PlasmacoreIntTable
{
  //---------------------------------------------------------------------------
  //  PlasmacoreIntTable::Reader
  //---------------------------------------------------------------------------
  struct Reader
  {
    PlasmacoreIntTable<DataType>*      table;
    PlasmacoreIntTableEntry<DataType>* cur_entry;
    int                      cur_bin_index;
    int                      index;

    Reader( PlasmacoreIntTable<DataType>* table, int starting_index )
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

    PlasmacoreIntTableEntry<DataType>* operator*() const
    {
      return cur_entry;
    }

    PlasmacoreIntTableEntry<DataType>* operator->() const
    {
      return cur_entry;
    }

    const Reader& operator++()
    {
      advance_entry();
      return *this;
    }

    PlasmacoreIntTableEntry<DataType>* read()
    {
      PlasmacoreIntTableEntry<DataType>* result = cur_entry;
      advance_entry();
      return result;
    }
  };

  //---------------------------------------------------------------------------
  //  PlasmacoreIntTable
  //---------------------------------------------------------------------------
  // PROPERTIES
  PlasmacoreIntTableEntry<DataType>** bins;
  int bin_count;
  int bin_mask;
  int count;

  PlasmacoreIntTable( int bin_count=16 )
    : bins(NULL), count(0)
  {
    int p2 = 1;
    while (p2 < bin_count) p2 <<= 1;
    this->bin_count = bin_count = p2;

    bin_mask = bin_count - 1;

    if (bins)
    {
      clear();
      delete bins;
    }

    bins = new PlasmacoreIntTableEntry<DataType>*[ bin_count ];
    memset( bins, 0, bin_count * sizeof(PlasmacoreIntTableEntry<DataType>*) );
  }

  ~PlasmacoreIntTable()
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

  void clear()
  {
    while (count) remove_another();
  }

  bool contains( int key )
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
    PlasmacoreIntTableEntry<DataType>* all_entries = 0;
    for (int i=0; i<bin_count; ++i)
    {
      PlasmacoreIntTableEntry<DataType>* cur = bins[ i ];
      while (cur)
      {
        PlasmacoreIntTableEntry<DataType>* next = cur->next_entry;
        cur->next_entry = all_entries;
        all_entries = cur;
        cur = next;
      }
    }

    // Recreate the bins
    delete bins;
    bin_count = new_bin_count;
    bin_mask = bin_count - 1;
    bins = new PlasmacoreIntTableEntry<DataType>*[ bin_count ];
    memset( bins, 0, bin_count * sizeof(PlasmacoreIntTableEntry<DataType>*) );

    // Reindex each entry
    while (all_entries)
    {
      PlasmacoreIntTableEntry<DataType>* cur = all_entries;
      all_entries = all_entries->next_entry;

      int bindex = cur->key & bin_mask;
      cur->next_entry = bins[ bindex ];
      bins[ bindex ] = cur;
    }
  }

  PlasmacoreIntTableEntry<DataType>* find( int key )
  {
    PlasmacoreIntTableEntry<DataType>* cur_entry = bins[ key & bin_mask ];

    while (cur_entry)
    {
      if (cur_entry->key == key)
      {
        return cur_entry;
      }
      cur_entry = cur_entry->next_entry;
    }
    return NULL;
  }


  DataType& get( int key )
  {
    PlasmacoreIntTableEntry<DataType>* cur_entry = bins[ key & bin_mask ];

    while (cur_entry)
    {
      if (cur_entry->key == key)
      {
        return cur_entry->value;
      }
      cur_entry = cur_entry->next_entry;
    }

    cur_entry = new PlasmacoreIntTableEntry<DataType>( key, DataType() );
    cur_entry->next_entry = bins[ key & bin_mask ];
    bins[ key & bin_mask ] = cur_entry;

    ++count;
    if (count > (bin_count << 2)) expand( bin_count * 2 );

    return cur_entry->value;
  }

  inline DataType& operator[]( int key ) { return get(key); }

  Reader reader() { return begin(); }

  DataType remove( int key )
  {
    PlasmacoreIntTableEntry<DataType>* cur_entry = bins[ key & bin_mask ];

    if ( !cur_entry ) return NULL;
    DataType result = cur_entry->value;

    if (cur_entry->key == key)
    {
      // Head of list is item to remove
      bins[ key & bin_mask ] = cur_entry->next_entry;
      delete cur_entry;
      --count;
      return result;
    }

    // After head of list
    PlasmacoreIntTableEntry<DataType>* next_entry = cur_entry->next_entry;
    while (next_entry)
    {
      if (next_entry->key == key)
      {
        cur_entry->next_entry = next_entry->next_entry;
        delete next_entry;
        --count;
        return result;
      }
      cur_entry = next_entry;
      next_entry = cur_entry->next_entry;
    }

    return NULL;  // should never happen
  }

  DataType remove_another()
  {
    if (bins)
    {
      for (int i=bin_mask; i>=0; --i)
      {
        PlasmacoreIntTableEntry<DataType>* cur_entry = bins[i];
        if (cur_entry)
        {
          PlasmacoreIntTableEntry<DataType>* next_entry = cur_entry->next_entry;
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

#endif // PLASMACORE_UTILITY_H
