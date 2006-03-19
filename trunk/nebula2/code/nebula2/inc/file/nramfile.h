#ifndef N_RAMFILE_H
//------------------------------------------------------------------------------
/**
    @class nRamFile
    @ingroup RAMFile

    Files that reside in memory.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nfile.h"
#include "kernel/nref.h"

class nSharedMemory;

//------------------------------------------------------------------------------
class nRamFile : public nFile
{
public:
    /// constructor
    nRamFile();
    /// destructor
    virtual ~nRamFile();

    /// Does file physically exists?
    virtual bool Exists(const nString& fileName) const;
    /// opens a file
    virtual bool Open(const nString& fileName, const nString& accessMode);
    /// closes the file
    virtual void Close();
    /// writes some bytes to the file
    virtual int Write(const void* buffer, int numBytes);
    /// reads some bytes from the file
    virtual int Read(void* buffer, int numBytes);
    /// gets actual position in file
    virtual int Tell() const;
    /// sets new position in file
    virtual bool Seek(int byteOffset, nSeekType origin);
    /// is the file at the end
    virtual bool Eof() const;
    /// get size of file in bytes
    virtual int GetSize() const;
    /// get the last write time
    virtual nFileTime GetLastWriteTime() const;

    /// Nebula path to shared memory objects.
    static const nString SharedMemoryRoot;

private:
    /// Full shared memory object name (with path) associated with short name `n'.
    nString GetFullSharedMemoryObjectName(const nString& n) const;
    /// Base name of filename `n'.
    nString GetBaseName(const nString& n) const;

    // Data
    nRef<nSharedMemory> refSharedMemory;
    int filePointerPos;
    bool fallBack;
};
//------------------------------------------------------------------------------
#endif
