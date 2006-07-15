//------------------------------------------------------------------------------
//  natomtable.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/natomtable.h"

nAtomTable* nAtomTable::Singleton = 0;

//------------------------------------------------------------------------------
/**
    nAtomTable constructor. Note that nAtomTable is a Singleton.
*/
nAtomTable::nAtomTable() :
    hashMap(512, 4)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
    nAtomTable destructor.
*/
nAtomTable::~nAtomTable()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

