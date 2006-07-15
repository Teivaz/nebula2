#ifndef N_NPKFILE_H
#define N_NPKFILE_H
//------------------------------------------------------------------------------
/**
    @class nNpkFile
    @ingroup NPKFile

    File access into a npk file.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nNpkFileServer;
class nNpkTocEntry;
class nNpkFile : public nFile
{
public:
    /// constructor
    nNpkFile();
    /// destructor
    virtual ~nNpkFile();

    /// Does file physically exists on disk?
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

private:
    nNpkTocEntry* tocEntry;         // associated npk toc entry object
    bool isNpkFile;                 // true if access into into an npk file
    bool isAsciiAccess;             // true if ascii access, else binary access
    int filePos;                    // current position in file
};
//------------------------------------------------------------------------------
#endif
