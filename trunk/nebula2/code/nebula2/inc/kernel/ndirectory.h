#ifndef N_DIRECTORY_H
#define N_DIRECTORY_H
//------------------------------------------------------------------------------
/**
    @class nDirectory
    @ingroup NebulaFileManagement

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
    enum nEntryType
    {
        FILE,
        DIRECTORY,
        INVALID,
    };

    /// constructor
    nDirectory(nFileServer2* server);
    /// destructor
    virtual ~nDirectory();

    /// opens a directory
    virtual bool Open(const char* dirName);
    /// closes the directory
    virtual void Close();
    /// get the full path name of the directory itself
    const char* GetPathName() const;
    /// check if the directory is empty
    virtual bool IsEmpty();
    /// set first entry as current
    virtual bool SetToFirstEntry();
    /// set next entry as current
    virtual bool SetToNextEntry();
    /// get name of current entry
    virtual const char* GetEntryName();
    /// get type of current entry
    virtual nEntryType GetEntryType();

protected:
    /// determines wether the directory is opened
    virtual bool IsOpen() const;

    nFileServer2* fs;
    bool empty;
    char path[N_MAXPATH];
    char apath[N_MAXPATH];

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
    return ((this->path[0] == 0) ? false : true);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDirectory::GetPathName() const
{
    n_assert(this->IsOpen());
    return this->apath;
}

//------------------------------------------------------------------------------
#endif
