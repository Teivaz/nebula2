#ifndef N_GUID_H
#define N_GUID_H
//------------------------------------------------------------------------------
/**
    @class nGuid

    Wraps a Globally Unique Identifier. Under Win32, this is a normal
    GUID. Other platform may implement something different. The actual
    GUID data is hidden inside the object and only accessible as string.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#if __WIN32__
#include <windows.h>
#include <Rpc.h>
#include <Rpcdce.h>
#else
#error "nGuid not implemented!"
#endif

//------------------------------------------------------------------------------
class nGuid
{
public:
    /// constructor
    nGuid();
    /// constructor with guid string
    nGuid(const char* guidStr);
    /// destructor
    ~nGuid();
    /// compare operator
    bool operator==(const nGuid& rhs) const;
    /// set guid as string
    void Set(const char* str);
    /// get guid as string
    const char* Get() const;
    /// generate a new guid
    void Generate();

private:
    nString guidString;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuid::nGuid()
{
    #if __WIN32__
    this->guidString = "00000000-0000-0000-0000-000000000000";
    #endif
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuid::nGuid(const char* guidStr)
{
    n_assert(guidStr);
    this->guidString = guidStr;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuid::~nGuid()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuid::operator==(const nGuid& rhs) const
{
    return (this->guidString == rhs.guidString);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuid::Set(const char* str)
{
    n_assert(str);
    this->guidString = str;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuid::Get() const
{
    return this->guidString.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuid::Generate()
{
    #if __WIN32__
        GUID guid;
        UuidCreate(&guid);
        uchar* guidStr;
        UuidToString(&guid, &guidStr);
        this->guidString = (const char*) guidStr;
        RpcStringFree(&guidStr);
    #endif
}

//------------------------------------------------------------------------------
#endif
