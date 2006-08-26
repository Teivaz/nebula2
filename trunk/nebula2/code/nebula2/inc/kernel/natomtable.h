#ifndef N_ATOMTABLE_H
#define N_ATOMTABLE_H
//------------------------------------------------------------------------------
/**
    @class nAtomTable
    @ingroup Kernel

    An atom table is where atoms store their strings.

    FIXME: hmm, the table can not shrink, is this even desireable???
    (shrinking the table would invalidate all nAtoms out there!).

    May be manually, expensive garbage collection pass would help? But in this
    case Atoms would need to become invalid some (and need to be able to
    revalidate themselves). This is only possible if the atom table
    stores pointers to all its atoms.... hmm hmm hmm...

    (C) 2004 RadonLabs GmbH
*/
#include "util/nhashmap.h"
#include "util/narray.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nAtomTable
{
public:
    /// constructor
    nAtomTable();
    /// destructor
    ~nAtomTable();
    /// return pointer to singleton instance
    static nAtomTable* Instance();
    /// convert a string into an index
    ushort StringToKey(const char* str);
    /// convert an index into a string
    const nString& KeyToString(ushort key) const;

private:
    static nAtomTable* Singleton;
    nHashMap hashMap;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAtomTable*
nAtomTable::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    Map string to 16 bit key.
*/
inline
ushort
nAtomTable::StringToKey(const char* str)
{
    return this->hashMap[str];
}

//------------------------------------------------------------------------------
/**
    Convert 16 bit key to string.
*/
inline
const nString&
nAtomTable::KeyToString(ushort key) const
{
    return this->hashMap[key];
}

//------------------------------------------------------------------------------
#endif

