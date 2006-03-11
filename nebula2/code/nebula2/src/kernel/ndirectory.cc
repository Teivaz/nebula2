//------------------------------------------------------------------------------
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ndirectory.h"

//------------------------------------------------------------------------------
/**
*/
nDirectory::nDirectory() : 
#if __WIN32__
    handle(0),
#endif
    empty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDirectory::~nDirectory()
{
    if (this->IsOpen())
    {
        n_printf("Warning: Directory destroyed before closing\n");
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    opens the specified directory

    @param dirName   the name of the directory to open
    @return          success

    history:
     - 30-Jan-2002   peter    created
*/
bool
nDirectory::Open(const nString& dirName)
{
    n_assert(!this->IsOpen());

    n_assert(dirName.IsValid());

    // mangle path name
    this->path = nFileServer2::Instance()->ManglePath(dirName);

#ifdef __WIN32__
    DWORD attr;
    bool retval;
    this->handle = NULL;

    // testen, ob File existiert und ein Dir ist...
    attr = GetFileAttributes(this->path.Get());
    if ((attr != 0xffffffff) && (attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        retval = true;
        this->empty = !(this->SetToFirstEntry());
    }
    else
    {
        retval = false;
        this->path.Clear();
    }

    return retval;
#else
    // FIXME LINUX NOT IMPLEMENTED YET
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
    closes the directory

    history:
     - 30-Jan-2002   peter    created
*/
void
nDirectory::Close()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    if (this->handle) 
    {
        FindClose(this->handle);
        this->handle = NULL;
    }
#else
    // FIXME: LINUX NOT IMPLEMENTED YET
#endif

    this->path.Clear();
}

//------------------------------------------------------------------------------
/**
    asks if directory is empty
  
    @return          true if empty

    history:
     - 30-Jan-2002   peter    created
*/
bool 
nDirectory::IsEmpty()
{
    n_assert(this->IsOpen());
    return this->empty;
}

//------------------------------------------------------------------------------
/**
    sets search index to first entry in directory
  
    @return          success

    history:
     - 30-Jan-2002   peter    created
*/
bool 
nDirectory::SetToFirstEntry()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    if (this->handle)
    {
        FindClose(this->handle);
    }

    nString tmpName = this->path;
    tmpName.Append("\\*.*");
    this->handle = FindFirstFile(tmpName.Get(), &(this->findData));

    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    while ((strcmp(this->findData.cFileName, "..") ==0) || (strcmp(this->findData.cFileName, ".") == 0))
    {
        if (!FindNextFile(this->handle, &this->findData))
        {
            return false;
        }
    }
    return true;
#else
    // FIXME: LINUX NOT IMPLEMENTED YET
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
    selects next directory entry
  
    @return          success

    history:
     - 30-Jan-2002   peter    created
*/
bool 
nDirectory::SetToNextEntry()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    bool suc = (FindNextFile(this->handle, &(this->findData)) != 0) ? true : false;

    return suc;
#else
    // FIXME: LINUX NOT IMPLEMENTED YET
    return false;
#endif
}

//------------------------------------------------------------------------------
/**
    gets name of actual directory entry
  
    @return          the name

    history:
     - 30-Jan-2002   peter    created
*/
nString 
nDirectory::GetEntryName()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    this->apath = this->path;
    this->apath.Append("/");
    this->apath.Append(this->findData.cFileName);
    return this->apath;
#else
    // FIXME: LINUX NOT IMPLEMENTED YET
    return 0;
#endif
}

//------------------------------------------------------------------------------
/**
    gets type of actual directory entry
  
    @return          FILE or DIRECTORY

    history:
     - 30-Jan-2002   peter    created
*/
nDirectory::EntryType 
nDirectory::GetEntryType()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    n_assert(this->handle);
    n_assert(this->handle != INVALID_HANDLE_VALUE);

    if(this->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return DIRECTORY;
    else
        return FILE;
#else
    // FIXME: LINUX NOT IMPLEMENTED YET
    return INVALID;
#endif
}

