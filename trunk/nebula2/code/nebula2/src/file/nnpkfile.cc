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
nNpkFile::nNpkFile() :
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
    Open the file, first try to open the file in the real filesystem. If this
    fails, try to open the file in a virtual filesystem contained in a npk file.
    If this fails too, return false.
*/
bool
nNpkFile::Open(const nString& filename, const nString& accessMode)
{
    n_assert(!this->IsOpen());
    n_assert(filename.IsValid());
    n_assert(accessMode.IsValid());

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
    nString absPath = nFileServer2::Instance()->ManglePath(filename);

    this->tocEntry = ((nNpkFileServer*)nFileServer2::Instance())->FindTocEntry(absPath);
    if (this->tocEntry && (nNpkTocEntry::FILE == this->tocEntry->GetType()))
    {
        this->isNpkFile     = true;
        this->isAsciiAccess = !accessMode.ContainsCharFromSet("bB");
        this->filePos       = 0;
        this->isOpen        = true;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nNpkFile::Exists(const nString& filename) const
{
    if (nFile::Exists(filename))
    {
        return true;
    }

    // not a conventional file, check if the npk file exists
    nString absPath = nFileServer2::Instance()->ManglePath(filename);

    nNpkTocEntry* tocEntry = ((nNpkFileServer*)nFileServer2::Instance())->FindTocEntry(absPath);
    if (tocEntry && (nNpkTocEntry::FILE == tocEntry->GetType()))
    {
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
    this->isOpen = false;
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
nNpkFile::Tell() const
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
/**
    Returns the file size.
*/
int
nNpkFile::GetSize() const
{
    n_assert(this->IsOpen());
    if (!this->isNpkFile)
    {
        return nFile::GetSize();
    }
    else
    {
        n_assert(this->tocEntry);
        return this->tocEntry->GetFileLength();
    }
}

//------------------------------------------------------------------------------
/**
    Returns true if file pointer is at end of file.
*/
bool
nNpkFile::Eof() const
{
    n_assert(this->IsOpen());
    if (!this->isNpkFile)
    {
        return nFile::Eof();
    }
    else
    {
        n_assert(this->tocEntry);
        return (this->filePos >= this->tocEntry->GetFileLength());
    }
}

//------------------------------------------------------------------------------
/**
    Returns the last write access time. This returns a 0-access time if
    this file is embedded into an NPK file.
*/
nFileTime
nNpkFile::GetLastWriteTime() const
{
    n_assert(this->IsOpen());
    if (!this->isNpkFile)
    {
        return nFile::GetLastWriteTime();
    }
    else
    {
        // return a dummy file time.
        nFileTime fileTime;
        return fileTime;
    }
}

//------------------------------------------------------------------------------
