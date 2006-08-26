#ifndef N_NPKFILEWRAPPER_H
#define N_NPKFILEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nNpkFileWrapper
    @ingroup NPKFile

    Wraps file operations on a npk file.

    (C) 2002 RadonLabs GmbH
*/
#include "util/nnode.h"
#include "file/nnpktoc.h"

//------------------------------------------------------------------------------
class nFile;
class nFileServer2;
class nNpkFileWrapper : public nNode
{
public:
    /// constructor
    nNpkFileWrapper();
    /// destructor
    ~nNpkFileWrapper();
    /// open the file and parse it
    bool Open(nFileServer2* fs, const char* path, const char* filename);
    /// close the file
    void Close();
    /// get a pointer to the binary file object
    nFile* GetBinaryFile() const;
    /// get a pointer to the ascii file object
    nFile* GetAsciiFile() const;
    /// get embedded npk toc object
    nNpkToc& GetTocObject();
    /// get absolute path name of npk file
    const nString& GetAbsPath() const;

private:
    /// parse table of contents
    bool ParseToc(const char* rootPath);
    /// read npk file header
    bool ReadHeader(nFile* file);
    /// read the table of content entries
    bool ReadTocEntries(nFile* file);

    nString absPath;            // absolute pathname of npk file
    nNpkToc toc;                // the table of contents of this npk file
    int dataOffset;             // start of the raw data in the npk file
    bool isOpen;
    nFile* binaryFile;          // binary file handle
    nFile* asciiFile;           // ascii file handle
};

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nNpkFileWrapper::GetAbsPath() const
{
    return this->absPath;
}

//------------------------------------------------------------------------------
#endif
