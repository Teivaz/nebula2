#ifndef N_FILETIMEPOSIX_H
#define N_FILETIMEPOSIX_H
//------------------------------------------------------------------------------
/**
    @class nFileTimePosix
    @ingroup File

    Implement POSIX nFileTime class. Please use the platform neutral
    nFileTime class for indirect access to _nFileTimePosix.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#if !defined(__LINUX__) && !defined(__MACOSX__)
#error "_nFileTimePosix: trying to compile POSIX class on hostile platform"
#endif

#include <time.h>

//------------------------------------------------------------------------------
class _nFileTimePosix
{
public:
    /// constructor
    _nFileTimePosix();
    /// operator ==
    friend bool operator ==(const _nFileTimePosix& a, const _nFileTimePosix& b);
        /// operator !=
    friend bool operator !=(const _nFileTimePosix& a, const _nFileTimePosix& b);
	/// operator >
    friend bool operator >(const _nFileTimePosix& a, const _nFileTimePosix& b);
    /// operator <
    friend bool operator <(const _nFileTimePosix& a, const _nFileTimePosix& b);

private:
    friend class nFile;
    time_t time;
};

//------------------------------------------------------------------------------
/**
*/
inline
_nFileTimePosix::_nFileTimePosix()
{
    time = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
operator ==(const _nFileTimePosix& a, const _nFileTimePosix& b)
{
    return (a.time == b.time);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
operator !=(const _nFileTimePosix& a, const _nFileTimePosix& b)
{
    return (a.time != b.time);
}


//------------------------------------------------------------------------------
/**
*/
inline
bool
operator >(const _nFileTimePosix& a, const _nFileTimePosix& b)
{
    return (a.time > b.time);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
operator <(const _nFileTimePosix& a, const _nFileTimePosix& b)
{
    return (a.time < b.time);
}

//------------------------------------------------------------------------------
#endif

