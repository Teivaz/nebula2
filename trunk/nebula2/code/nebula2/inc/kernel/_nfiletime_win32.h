#ifndef N_FILETIMEWIN32_H
#define N_FILETIMEWIN32_H
//------------------------------------------------------------------------------
/**
    @class nFileTimeWin32
    @ingroup File

    Implement Win32 specific nFileTime class. Please use the platform neutral
    nFileTime class for indirect access to _nFileTimeWin32.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef __WIN32__
#error "_nFileTimeWin32: trying to compile Win32 class on hostile platform"
#endif

//------------------------------------------------------------------------------
class _nFileTimeWin32
{
public:
    /// constructor
    _nFileTimeWin32();
    /// operator ==
    friend bool operator ==(const _nFileTimeWin32& a, const _nFileTimeWin32& b);
    /// operator !=
    friend bool operator !=(const _nFileTimeWin32& a, const _nFileTimeWin32& b);
    /// operator >
    friend bool operator >(const _nFileTimeWin32& a, const _nFileTimeWin32& b);
    /// operator <
    friend bool operator <(const _nFileTimeWin32& a, const _nFileTimeWin32& b);

private:
    friend class nFile;
    FILETIME time;
};

//------------------------------------------------------------------------------
/**
*/
inline
_nFileTimeWin32::_nFileTimeWin32()
{
    time.dwLowDateTime = 0;
    time.dwHighDateTime = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator ==(const _nFileTimeWin32& a, const _nFileTimeWin32& b)
{
    return (0 == CompareFileTime(&(a.time), &(b.time)));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator !=(const _nFileTimeWin32& a, const _nFileTimeWin32& b)
{
    return (0 != CompareFileTime(&(a.time), &(b.time)));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator >(const _nFileTimeWin32& a, const _nFileTimeWin32& b)
{
    return (1 == CompareFileTime(&(a.time), &(b.time)));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator <(const _nFileTimeWin32& a, const _nFileTimeWin32& b)
{
    return (-1 == CompareFileTime(&(a.time), &(b.time)));
}

//------------------------------------------------------------------------------
#endif


