#ifndef N_FILESERVER2_H
#define N_FILESERVER2_H
//------------------------------------------------------------------------------
/**
    @class nFileServer2
    @ingroup NebulaFileManagement

    New fileserver.

    Provides functions for creating file and directory objects and assigns.

    Please note that Nebula no longer knows the concept of a current working
    directory!

    See also @ref N2ScriptInterface_nfileserver2

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nFileServer2
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nDirectory;
class nFile;
class N_PUBLIC nFileServer2 : public nRoot
{
public:
    /// constructor
    nFileServer2();
    /// destructor
    virtual ~nFileServer2();

	/// sets a path alias
	bool SetAssign(const char* assignName, const char* pathName);
	/// gets a path alias
	const char* GetAssign(const char* assignName);
	/// expand path alias to real path
	const char* ManglePath(const char* pathName, char* buf, int bufSize);

	/// creates a new nDirectory object
	virtual nDirectory* NewDirectoryObject();
	/// creates a new nFile object
    virtual nFile* NewFileObject();

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

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

protected:
    /// cleanup a path name in place
    void CleanupPathName(char* path);
    
private:
    /// initialize standard home: assign
    void InitHomeAssign();
    /// initialize the standard bin: assign
    void InitBinAssign();
    
    nRef<nRoot> assignDir;

    int bytesRead;
    int bytesWritten;
    int numSeeks;
};

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
