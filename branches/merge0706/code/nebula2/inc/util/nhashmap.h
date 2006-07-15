#ifndef N_HASHARRAY_H
#define N_HASHARRAY_H
//------------------------------------------------------------------------------
/**
    @class nHashArray
    @ingroup Util

    This associates a 16 bit key with a string.

    FIXME: does not allow to erase entries (this would invalidate indices,
    probably add some sort of garbage collection pass?).
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/narray.h"
#include "util/nfixedarray.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nHashMap
{
public:
    /// default constructor
    nHashMap();
    /// constructor with initial hash size and rehash threshold
    nHashMap(int s, int r);
    /// destructor
    ~nHashMap();
    /// adds a string to the hash map, returns index of string
    ushort AddString(const char* str);
    /// map string to 16 bit key
    ushort operator[](const char* index);
    /// map 16 bit key to string
    const nString& operator[](ushort key) const;
    /// check if entry exists
    bool Exists(const char* index) const;
    /// access to dense array (can be indexed with 16 bit key)
    const nArray<nString>& GetDenseArray() const;
    /// return current hash size
    int GetHashSize() const;

private:
    /// disabled copy constructor
    nHashMap(const nHashMap& rhs);
    /// disabled assignment operator
    void operator=(const nHashMap& rhs);
    /// compute a hash value from a string
    int HashIndex(const char* str) const;
    /// rehash the table using a new hash size
    void Rehash(int newSize);

    int rehashThreshold;
    nFixedArray<nArray<ushort> >* hashTable;
    nArray<nString> stringArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nHashMap::nHashMap() :
    rehashThreshold(8),
    stringArray(128, 128)
{
    this->hashTable = n_new(nFixedArray<nArray<ushort> >(128));
    this->stringArray.SetFlags(nArray<nString>::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
nHashMap::nHashMap(int s, int r) :
    rehashThreshold(r),
    stringArray(s, s)
{
    this->hashTable = n_new(nFixedArray<nArray<ushort> >(s));
    this->stringArray.SetFlags(nArray<nString>::DoubleGrowSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
nHashMap::~nHashMap()
{
    n_assert(this->hashTable);
    n_delete(this->hashTable);
    this->hashTable = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nHashMap::GetHashSize() const
{
    return this->hashTable->Size();
}

//------------------------------------------------------------------------------
/**
    The copy constructor is private and thus disabled.
*/
inline
nHashMap::nHashMap(const nHashMap& /*rhs*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    The assignment operator is private and thus disabled.
*/
inline
void
nHashMap::operator=(const nHashMap& /*rhs*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute hash value from string.
*/
inline
int 
nHashMap::HashIndex(const char* str) const
{
    n_assert(str);

    int hashSize = this->GetHashSize();
    int i = 0;
    int j = 1;
    char c;
    while ((c = *str++)) 
    {
        i += ((uchar)c) * j++; 
    }
    return (i % hashSize);
}

//------------------------------------------------------------------------------
/**
    Rehash the table with a new hash size.
*/
inline
void
nHashMap::Rehash(int newHashSize)
{
    n_printf("nHashMap: rehashing to size %d\n", newHashSize);

    nFixedArray<nArray<ushort> >* oldTable = this->hashTable;
    n_assert(oldTable);
    this->hashTable = n_new(nFixedArray<nArray<ushort> >(newHashSize));
    
    int oldTableIndex;
    int oldTableSize = oldTable->Size();
    for (oldTableIndex = 0; oldTableIndex < oldTableSize; oldTableIndex++)
    {
        const nArray<ushort>& entryArray = (*oldTable)[oldTableIndex];
        int entryArraySize = entryArray.Size();
        int j;
        for (j = 0; j < entryArraySize; j++)
        {
            ushort stringIndex = entryArray[j];
            int hashIndex = this->HashIndex(this->stringArray[stringIndex].Get());
            (*this->hashTable)[hashIndex].Append(stringIndex);
        }
    }
    n_delete(oldTable);
}

//------------------------------------------------------------------------------
/**
    The [] operator which maps a string to ushort key.
*/
inline
ushort
nHashMap::operator[](const char* indexString)
{
    n_assert(indexString);

    // compute hash index
    int hashIndex = this->HashIndex(indexString);
    nArray<ushort>* entryArray = &((*this->hashTable)[hashIndex]); 

    // check for existing entry
    int i;
    int num = entryArray->Size();

    // rehash entire table if rehash threshold for this entry is reached
    if (num > this->rehashThreshold)
    {
        this->Rehash(this->GetHashSize() * 2);
        hashIndex = this->HashIndex(indexString);
        entryArray = &((*this->hashTable)[hashIndex]);
        num = entryArray->Size();
    }
    
    // check if the string already exists, this involves string compares
    for (i = 0; i < num; i++)
    {
        ushort stringIndex = (*entryArray)[i];
        if (0 == strcmp(this->stringArray[stringIndex].Get(), indexString))
        {
            // yep exists
            return stringIndex;
        }
    }
    // fallthrough: new string, add string to dense string array and
    // add new string index to hash table
    this->stringArray.Append(indexString);  // FIXME: calls hidden nString constructor!
    ushort newStringIndex = this->stringArray.Size() - 1;
    (*this->hashTable)[hashIndex].Append(newStringIndex);
    return newStringIndex;
}

//------------------------------------------------------------------------------
/**
    Adds a string to the hash map and return its index. This is just an alias
    for the operator[] which takes a const char* as index.
*/
inline
ushort
nHashMap::AddString(const char* str)
{
    return (*this)[str];
}

//------------------------------------------------------------------------------
/**
    The [] operator which maps a key back to its string. This is very fast.
*/
inline
const nString&
nHashMap::operator[](ushort key) const
{
    return this->stringArray[key];
}

//------------------------------------------------------------------------------
/**
    Check if a string exists in the hash map.
*/
inline
bool
nHashMap::Exists(const char* indexString) const
{
    n_assert(indexString);

    // compute hash index
    int hashIndex = this->HashIndex(indexString);
    const nArray<ushort>& entryArray = (*this->hashTable)[hashIndex]; 
    int i;
    int num = entryArray.Size();
    for (i = 0; i < num; i++)
    {
        ushort stringIndex = entryArray[i];
        if (0 == strcmp(this->stringArray[stringIndex].Get(), indexString))
        {
            // yep exists
            return true;
        }
    }
    // fallthrough: does not exist
    return false;
}

//------------------------------------------------------------------------------
/**
    Provide access to dense array. The 16 bit keys can be used as indices
    into this array.
*/
inline
const nArray<nString>&
nHashMap::GetDenseArray() const
{
    return this->stringArray;
}

//------------------------------------------------------------------------------
#endif

