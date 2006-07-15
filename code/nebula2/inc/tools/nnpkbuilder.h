#ifndef N_NPKBUILDER_H
#define N_NPKBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nNpkBuilder
    @ingroup Tools

    A utility class which supports packing, diffing, and unpacking
    npk files.
    
    (C) 2004 RadonLabs GmbH
*/
#include "util/nstring.h"
#include "file/nnpktoc.h"
#include "kernel/nfile.h"
#include "kernel/ndirectory.h"

//------------------------------------------------------------------------------
class nNpkBuilder
{
public:
    /// error codes
    enum Error
    {
        NoError = 0,
        CannotOpenSourceDirectory,
        CannotOpenNpkFile,
    };

    /// constructor
    nNpkBuilder();
    /// destructor
    ~nNpkBuilder();
    /// pack a directory into an npk file
    bool Pack(const nString& rootPath, const nString& dirName, const nString& npkName, bool noRootName);
    /// get error code
    Error GetError() const;

private:
    /// recursively generate table of contents object
    bool GenerateToc(nDirectory* dir, const nString& dirName);
    /// write table of contents to file
    bool WriteToc(bool noRootName);
    /// recursively write toc entries to file
    bool WriteTocEntry(nNpkTocEntry* tocEntry, bool noRootName);
    /// write data block to npk file
    bool WriteData();
    /// recursively write data block for entries to npk file
    bool WriteEntryData(nNpkTocEntry* tocEntry);
    /// set error code
    void SetError(Error e);

    nNpkToc* tocObject;
    nFile* npkFile;
    int fileOffset;
    int dataBlockStart;
    int dataBlockOffset;
    int dataSize;
    Error errorCode;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nNpkBuilder::SetError(Error e)
{
    this->errorCode = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
nNpkBuilder::Error
nNpkBuilder::GetError() const
{
    return this->errorCode;
}

//------------------------------------------------------------------------------
#endif
