#ifndef N_HASHTABLE_H
#define N_HASHTABLE_H
//------------------------------------------------------------------------------
/**
    @class nHashTable
    @ingroup NebulaDataTypes

    Implements a simple string hash table.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_STRLIST_H
#include "util/nstrlist.h"
#endif

//------------------------------------------------------------------------------
class nHashTable 
{
public:
    /// constructor
    nHashTable(int size);
    /// destructor
    ~nHashTable();
    /// add an entry to the hashtable
    void Add(nStrNode* n);
    /// search hash table for entry
    nStrNode* Find(const char* str) const;

private:
    int htable_size;
    nStrList *htable;
};

//------------------------------------------------------------------------------
/**
*/
inline 
nHashTable::nHashTable(int size)
{
    this->htable_size = size;
    this->htable = n_new nStrList[size];
}

//------------------------------------------------------------------------------
/**
*/
inline nHashTable::~nHashTable()
{
    n_delete[] this->htable;
}

//------------------------------------------------------------------------------
/**
*/
static 
inline 
int hash(const char *str, int htable_size)
{
    int i = 0;
    int j = 1;
    char c;
    while ((c = *str++)) i += ((uchar)c) * j++; 
    return (i % htable_size);
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nHashTable::Add(nStrNode* n)
{
    int h_index = hash(n->GetName(), this->htable_size);
    this->htable[h_index].AddHead(n);
}

//------------------------------------------------------------------------------
/**
*/
inline 
nStrNode*
nHashTable::Find(const char* str) const
{
    int h_index = hash(str, this->htable_size);
    return this->htable[h_index].Find(str);
}

//------------------------------------------------------------------------------
#endif
