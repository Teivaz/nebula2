#define N_IMPLEMENTS nNpkFile
//------------------------------------------------------------------------------
//  nnpkfile.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "file/nnpkfile.h"
#include "file/nnpkfileserver.h"
#include "file/nnpkfilewrapper.h"

//------------------------------------------------------------------------------
/**
*/
nNpkFile::nNpkFile(nFileServer2* fs) :
    nFile(fs),
    npkFileServer((nNpkFileServer*) fs),
    isNpkFile(false),
    isAsciiAccess(false),
    tocEntry(0),
    filePos(0)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
*/
nNpkFile::~nNpkFile()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkFile::IsOpen()
{
    if (this->isNpkFile)
    {
        return (this->tocEntry != 0);
    }
    else
    {
        return nFile::IsOpen();
    }
}

//------------------------------------------------------------------------------
/**
    Open the file, first try to open the file in the real filesystem. If this
    fails, try to open the file in a virtual filesystem contained in a npk file.
    If this fails too, return false.
*/
bool
nNpkFile::Open(const char* filename, const char* accessMode)
{
    n_assert(!this->IsOpen());
    n_assert(filename);
    n_assert(accessMode);

    // some inits
    this->isNpkFile     = false;
    this->isAsciiAccess = false;
    this->tocEntry      = 0;
    this->filePos       = 0;
    this->lineNumber    = 0;

    // try to open as conventional file
    if (nFile::Open(filename, accessMode))
    {
        return true;
    }

    // not a conventional file, try to open as npk file
    char absPath[N_MAXPATH];
    this->npkFileServer->ManglePath(filename, absPath, sizeof(absPath));

    this->tocEntry = this->npkFileServer->FindTocEntry(absPath);
    if (this->tocEntry && (nNpkTocEntry::FILE == this->tocEntry->GetType()))
    {
        this->isNpkFile     = true;
        this->isAsciiAccess = !(strchr(accessMode, 'b'));
        this->filePos       = 0;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Close the file. If it is a npk file, this is basically a no-op.
*/
void
nNpkFile::Close()
{
    n_assert(this->IsOpen());

    if (!this->isNpkFile)
    {
        nFile::Close();
        return;
    }

    this->isNpkFile = 0;
    this->isAsciiAccess = 0;
    this->tocEntry = 0;
    this->filePos = 0;
}

//------------------------------------------------------------------------------
/**
    Read bytes from the file.
*/
int
nNpkFile::Read(void* buffer, int numBytes)
{
    if (!this->isNpkFile)
    {
        return nFile::Read(buffer, numBytes);
    }

    n_assert(this->IsOpen());
    n_assert(this->tocEntry);

    // read access goes through a npk file:
    //  - get the actual file handle from the toc entry (binary or ascii)
    //  - clamp numBytes if necessary (if read would go past end of file)
    //  - adjust the current file offset
    //  - do the read operation
    //  - store new file pos
    nNpkFileWrapper* fileWrapper = this->tocEntry->GetFileWrapper();
    n_assert(fileWrapper);
    nFile* file;
    if (this->isAsciiAccess)
    {
        file = fileWrapper->GetAsciiFile();
    }
    else
    {
        file = fileWrapper->GetBinaryFile();
    }
    n_assert(file);

    // clamp numBytes if read would go past end of file
    int fileLength = tocEntry->GetFileLength();
    n_assert(this->filePos <= fileLength);

    int endPos = this->filePos + numBytes;
    if (endPos >= fileLength)
    {
        numBytes = fileLength - this->filePos;
    }

    // adjust the file position and read bytes
    file->Seek(tocEntry->GetFileOffset() + this->filePos, START);
    int bytesRead = file->Read(buffer, numBytes);
    this->filePos += bytesRead;
    return bytesRead;
}

//------------------------------------------------------------------------------
/**
    Writing to npk files is not supported!
*/
int
nNpkFile::Write(const void* buffer, int numBytes)
{
    n_assert(this->IsOpen());

    if (!this->isNpkFile)
    {
        return nFile::Write(buffer, numBytes);
    }

    n_error("nNpkFile: Writing to npk files not supported!");
    return 0;
}

//------------------------------------------------------------------------------
/**
    Return current file position.
*/
int
nNpkFile::Tell()
{
    n_assert(this->IsOpen());

    if (!this->isNpkFile)
    {
        return nFile::Tell();
    }
    else
    {
        return this->filePos;
    }
}

//------------------------------------------------------------------------------
/**
    Change current file position.
*/
bool
nNpkFile::Seek(int byteOffset, nSeekType origin)
{
    n_assert(this->IsOpen());
    
    if (!this->isNpkFile)
    {
        return nFile::Seek(byteOffset, origin);
    }
    else
    {
        n_assert(this->tocEntry);

        bool retval = false;
        int fileLength = this->tocEntry->GetFileLength();
        int seekPos = 0;
        switch (origin)
        {
            case CURRENT:
                seekPos = this->filePos + byteOffset;
                break;

            case START:
                seekPos = byteOffset;
                break;

            case END:
                seekPos = fileLength + byteOffset;
                break;
        }

        // clamp endPos
        if (seekPos >= fileLength)
        {
            // end of file reached
            this->filePos = fileLength;
            retval = false;
        }
        else if (seekPos < 0)
        {
            // before start of file
            this->filePos = 0;
            retval = true;
        }
        else
        {
            // normal case
            this->filePos = seekPos;
            retval = true;
        }

        return true;
    }
}

//------------------------------------------------------------------------------
