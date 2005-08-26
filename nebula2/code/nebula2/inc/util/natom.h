#ifndef N_ATOM_H
#define N_ATOM_H
//------------------------------------------------------------------------------
/**
    @class nAtom
    @ingroup Util

    Atoms are shared read-only strings which only require 2 bytes of
    memory per instance and have extremly fast compare and copy operations. 

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/natomtable.h"

//------------------------------------------------------------------------------
class nAtom
{
public:
    /// default constructor
    nAtom();
    /// constructor from char pointer, in default global table
    nAtom(const char* str);
    /// copy constructor
    nAtom(const nAtom& rhs);
    /// destructor
    ~nAtom();
    /// assignment operator
    void operator=(const nAtom& rhs);
    /// assignment operator with char pointer
    void operator=(const char* rhs);
    /// equality operator
    bool operator==(const nAtom& rhs) const;
    /// equality operator with char pointer
    bool operator==(const char* rhs) const;
    /// inequality operator
    bool operator!=(const nAtom& rhs) const;
    /// inequality operator with char pointer
    bool operator!=(const char* rhs) const;
    /// return string as const char*
    const char* AsChar() const;
    /// return string as const nString& 
    const nString& AsString() const;
    /// cast to nString
    const nString& operator*() const;

protected:
    enum
    {
        InvalidKey = 0xffff,
    };
    ushort key;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAtom::nAtom() :
    key(InvalidKey)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAtom::nAtom(const char* str)
{
    this->key = nAtomTable::Instance()->StringToKey(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
nAtom::nAtom(const nAtom& rhs)
{
    this->key = rhs.key;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAtom::~nAtom()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAtom::operator=(const nAtom& rhs)
{
    this->key = rhs.key;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAtom::operator=(const char* rhs)
{
    this->key = nAtomTable::Instance()->StringToKey(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAtom::operator==(const nAtom& rhs) const
{
    return this->key == rhs.key;
}

//------------------------------------------------------------------------------
/**
    WARNING: this ends up as a normal string compare!
*/
inline
bool
nAtom::operator==(const char* rhs) const
{
    n_assert(rhs);
    return (0 == strcmp(nAtomTable::Instance()->KeyToString(this->key).Get(), rhs));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAtom::operator!=(const nAtom& rhs) const
{
    return this->key != rhs.key;
}

//------------------------------------------------------------------------------
/**
    WARNING: this ends up as a normal string compare!
*/
inline
bool
nAtom::operator!=(const char* rhs) const
{
    n_assert(rhs);
    return (0 != strcmp(nAtomTable::Instance()->KeyToString(this->key).Get(), rhs));
}

//------------------------------------------------------------------------------
/**
    Return string as const char*
*/
inline
const char*
nAtom::AsChar() const
{
    return nAtomTable::Instance()->KeyToString(this->key).Get();
}

//------------------------------------------------------------------------------
/**
    Return string as const nString&
*/
inline
const nString&
nAtom::AsString() const
{
    return nAtomTable::Instance()->KeyToString(this->key);
}

//------------------------------------------------------------------------------
/**
    Cast to "const nString&".
*/
inline
const nString&
nAtom::operator*() const
{
    return nAtomTable::Instance()->KeyToString(this->key);
}

//------------------------------------------------------------------------------
#endif        

