#ifndef N_DIRECTORY_H
#define N_DIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nDirectory
    @ingroup File

    wrapper for directory functions

    provides functions for searching directories

    (C) 2002 RadonLabs GmbH
*/

#include "kernel/nfileserver2.h"

#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#elif defined(__WIN32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//------------------------------------------------------------------------------
class nDirectory
{
public:
    enum EntryType
    {
        FILE,
        DIRECTORY,
        INVALID,
    };

    /// destructor
    virtual ~nDirectory();

    /// opens a directory
    virtual bool Open(const nString& dirName);
    /// closes the directory
    virtual void Close();
    /// determines whether the directory is open
    virtual bool IsOpen() const;
    /// get the full path name of the directory itself
    nString GetPathName() const;
    /// check if the directory is empty
    virtual bool IsEmpty();
    /// set first entry as current
    virtual bool SetToFirstEntry();
    /// set next entry as current
    virtual bool SetToNextEntry();
    /// get full path name of current entry
    virtual nString GetEntryName();
    /// get type of current entry
    virtual EntryType GetEntryType();

protected:
    friend class nFileServer2;

    /// NOTE: constructor is private because only nFileServer2 may create objects
    nDirectory();

    bool empty;
    nString path;
    nString apath;

#ifdef __WIN32__
    /// win32 directory search handle
    HANDLE handle;
    /// win32 search result
    WIN32_FIND_DATA findData;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nDirectory::IsOpen() const
{
    return !this->path.IsEmpty();
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nDirectory::GetPathName() const
{
    n_assert(this->IsOpen());
    return this->path;
}

//------------------------------------------------------------------------------
#endif
