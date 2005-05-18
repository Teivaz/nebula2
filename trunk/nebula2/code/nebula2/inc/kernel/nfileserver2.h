#ifndef N_FILESERVER2_H
#define N_FILESERVER2_H
//------------------------------------------------------------------------------
/**
    @class nFileServer2
    @ingroup File

    @brief Central server object of Nebula2's file system subsystem.
    Provides functions for creating file and directory objects and assigns.

    Please note that Nebula no longer knows the concept of a current working
    directory!

    See also @ref N2ScriptInterface_nfileserver2

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "util/nstring.h"
#ifdef __WIN32__
#include "kernel/nshell32wrapper.h"
#endif

//------------------------------------------------------------------------------
class nDirectory;
class nFile;
class nFileNode;

class nFileServer2 : public nRoot
{
public:
    /// constructor
    nFileServer2();
    /// destructor
    virtual ~nFileServer2();
    /// get instance pointer
    static nFileServer2* Instance();

    /// sets a path alias
    bool SetAssign(const char* assignName, const char* pathName);
    /// gets a path alias
    const char* GetAssign(const char* assignName);
    /// Reset assign repository to default values.
    void ResetAssigns();
    /// expand path alias to real path
    nString ManglePath(const char* pathName);
    /// make path components
    virtual bool MakePath(const char* path);
    /// copy a file
    virtual bool CopyFile(const char* from, const char* to);
    /// delete a file
    virtual bool DeleteFile(const char* filename);
    /// delete an empty directory
    virtual bool DeleteDirectory(const char* dirName);
    /// compute the Crc checksum for a file
    virtual bool Checksum(const char* filename, uint& crc);
    /// set read only status of a file
    virtual void SetFileReadOnly(const char* filename, bool readOnly);
    /// get read only status of a file
    virtual bool IsFileReadOnly(const char* filename);

    /// creates a new nDirectory object
    virtual nDirectory* NewDirectoryObject() const;
    /// creates a new nFile object
    virtual nFile* NewFileObject() const;
    /// check if file exists
    virtual bool FileExists(const char* pathName) const;
    /// check if directory exists
    virtual bool DirectoryExists(const char* pathName) const;
    /// creates a file node (only useful for scripting languages)
    virtual nFileNode* CreateFileNode(const char* name);

    /// reset statistics
    void ResetStatistics();
    /// add read bytes 
    void AddBytesRead(int b);
    /// add written bytes
    void AddBytesWritten(int b);
    /// add seek operation to stats
    void AddSeek();
    /// get bytes read since last reset
    int GetBytesRead() const;
    /// get bytes written since last reset
    int GetBytesWritten() const;
    /// get number of seeks
    int GetNumSeeks() const;

protected:
    /// cleanup a path name in place
    void CleanupPathName(nString& str);
    
private:
    /// Initialize assign repository.
    void InitAssigns();
    /// initialize standard home: assign
    void InitHomeAssign();
    /// initialize the standard bin: assign
    void InitBinAssign();
    /// initialize the standard user: assign
    void InitUserAssign();
    /// initialize the standard temp: assign
    void InitTempAssign();

    static nFileServer2* Singleton;

    nRef<nRoot> assignDir;

    int bytesRead;
    int bytesWritten;
    int numSeeks;

    #ifdef __WIN32__
    nShell32Wrapper shell32Wrapper;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nFileServer2*
nFileServer2::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    Reset statistics.
*/
inline
void
nFileServer2::ResetStatistics()
{
    this->bytesRead = 0;
    this->bytesWritten = 0;
    this->numSeeks = 0;
}

//------------------------------------------------------------------------------
/**
    Add read bytes to statistics.
*/
inline
void
nFileServer2::AddBytesRead(int b)
{
    this->bytesRead += b;
}

//------------------------------------------------------------------------------
/**
    Add read bytes written to statistics.
*/
inline
void
nFileServer2::AddBytesWritten(int b)
{
    this->bytesWritten += b;
}

//------------------------------------------------------------------------------
/**
    Add a seek op to stats.
*/
inline
void
nFileServer2::AddSeek()
{
    this->numSeeks++;
}

//------------------------------------------------------------------------------
/**
    Get bytes read since last ResetStatistics()
*/
inline
int
nFileServer2::GetBytesRead() const
{
    return this->bytesRead;
}

//------------------------------------------------------------------------------
/**
    Get bytes written since last ResetStatistics()
*/
inline
int
nFileServer2::GetBytesWritten() const
{
    return this->bytesWritten;
}

//------------------------------------------------------------------------------
/**
    Get number of seeks since last ResetStatistics().
*/
inline
int
nFileServer2::GetNumSeeks() const
{
    return this->numSeeks;
}

//------------------------------------------------------------------------------
#endif
