#ifndef N_GUID_H
#define N_GUID_H
//------------------------------------------------------------------------------
/**
    @class nGuid
    @ingroup Kernel

    @brief Wraps a Globally Unique Identifier.  The actual
    GUID data is hidden inside the object and only accessible as string.

    Under Win32, this is a normal GUID.
    Under GNU/Linux, libuuid from the e2fsprogs package is used.
    Other platforms may implement something different.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

#if __WIN32__
#include <windows.h>
#include <Rpc.h>
#include <Rpcdce.h>
#elif defined(__LINUX__)
#include <uuid/uuid.h>
#elif defined(__MACOSX__)
#include <CoreFoundation/CFUUID.h>
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
    #if defined(__WIN32__) || defined(__LINUX__) || defined(__MACOSX__)
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
    #if defined(__WIN32__)
        GUID guid;
        UuidCreate(&guid);
        uchar* guidStr;
        UuidToString(&guid, &guidStr);
        this->guidString = (const char*) guidStr;
        RpcStringFree(&guidStr);
    #elif defined(__LINUX__)
        uuid_t guid;
        uuid_generate(guid);
        n_assert(!uuid_is_null(guid));

        char guidstr[37];
        uuid_unparse(guid, guidstr);
        this->guidString = guidstr;
    #elif defined(__MACOSX__)
        CFUUIDRef guid = CFUUIDCreate(NULL);
        CFStringRef guidstr = CFUUIDCreateString(NULL, guid);
        CFRelease(guid);

        char guidcstr[37];
        CFStringGetCString(guidstr, guidcstr,
                           sizeof(guidcstr), kCFStringEncodingASCII);
        CFRelease(guidstr);

        this->guidString = guidcstr;
    #endif
}

//------------------------------------------------------------------------------
#endif
