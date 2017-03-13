//=============================================================================
//  PlasmacoreIntTable.h
//
//  v1.1.3 - 2016.03.05 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================
#ifndef PLASMACORE_INT_TABLE_H
#define PLASMACORE_INT_TABLE_H

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

#endif // PLASMACORE_INT_TABLE_H

