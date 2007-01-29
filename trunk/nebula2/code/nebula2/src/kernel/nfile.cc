//------------------------------------------------------------------------------
//  nfile.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

#if defined(__LINUX__) || defined(__MACOSX__)
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

//------------------------------------------------------------------------------
/**
    history:
     - 30-Jan-2002   peter   created
     - 11-Feb-2002   floh    Linux stuff
*/
nFile::nFile() :
    lineNumber(0),
    isOpen(false)
{
#ifdef __WIN32__
    this->handle = 0;
#else
    this->fp = 0;
#endif
}

//------------------------------------------------------------------------------
/**

    history:
     - 30-Jan-2002   peter    created
*/
nFile::~nFile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Check if file exists physically on disk by opening it in read-only mode.
    Close file if it was opened.

     - 05-Jan-05   floh    Bugfix: missing GENERIC_READ access mode didn't work in Win98
     - 12-Oct-05   floh    access move back to 0, Win98 is no longer relevant
*/
bool
nFile::Exists(const nString& fileName) const
{
    n_assert(fileName.IsValid());

    nString fullName = nFileServer2::Instance()->ManglePath(fileName);

#ifdef __WIN32__
    HANDLE fh = CreateFile(fullName.Get(),       // filename
                           0,                    // don't actually open the file, just check for existence
                           FILE_SHARE_READ,      // share mode
                           0,                    // security flags
                           OPEN_EXISTING,        // what to do if file doesn't exist
                           FILE_ATTRIBUTE_NORMAL,  // flags'n'attributes
                           0);                   // template file
    if (fh != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fh);
        return true;
    }
    else
    {
        return false;
    }
#else
    FILE* fp = fopen(fullName.Get(), "r");
    if (fp != 0)
    {
        fclose(fp);
        return true;
    }
    else
    {
        return false;
    }
#endif
    return false;
}

//------------------------------------------------------------------------------
/**
    opens the specified file

    @param fileName     the name of the file to open
    @param accessMode   the access mode ("(r|w|a)[+]")
    @return             success

    history:
     - 30-Jan-2002   peter   created
     - 11-Feb-2002   floh    Linux stuff
     - 12-Oct-2005   floh    fixed read mode to also open a file which is
                             already open by another application for writing
*/
bool
nFile::Open(const nString& fileName, const nString& accessMode)
{
    n_assert(!this->IsOpen());

    n_assert(fileName.IsValid());
    n_assert(accessMode.IsValid());

    nString mangledPath = nFileServer2::Instance()->ManglePath(fileName);
    this->lineNumber = 0;

#ifdef __WIN32__
    DWORD access = 0;
    DWORD disposition = 0;
    DWORD shareMode = 0;
    if (accessMode.ContainsCharFromSet("rR"))
    {
        access |= GENERIC_READ;
    }
    if (accessMode.ContainsCharFromSet("wW"))
    {
        access |= GENERIC_WRITE;
    }
    if (access & GENERIC_WRITE)
    {
        disposition = CREATE_ALWAYS;
        shareMode = FILE_SHARE_READ;    // allow reading the file in write mode
    }
    else
    {
        disposition = OPEN_EXISTING;
        shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE; // allow read/write in read mode
    }
    this->handle = CreateFile(mangledPath.Get(),    // filename
                              access,               // access mode
                              shareMode,            // share mode
                              0,                    // security flags
                              disposition,          // what to do if file doesn't exist
                              FILE_ATTRIBUTE_NORMAL,   // flags'n'attributes
                              0);                   // template file
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        this->handle = 0;
        return false;
    }
#else
    this->fp = fopen(mangledPath.Get(), accessMode.Get());
    if (!this->fp)
    {
        return false;
    }
#endif
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    closes the file

    history:
     - 30-Jan-2002   peter   created
     - 11-Feb-2002   floh    Linux stuff
*/
void
nFile::Close()
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    if (this->handle)
    {
        CloseHandle(this->handle);
        this->handle = 0;
    }
#else
    if (this->fp)
    {
        fclose(this->fp);
        this->fp = 0;
    }
#endif
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    writes a number of bytes to the file

    @param buffer       buffer with data
    @param numBytes     number of bytes to write
    @return             number of bytes written

    history:
     - 30-Jan-2002   peter    created
     - 11-Feb-2002   floh    Linux stuff
*/
int
nFile::Write(const void* buffer, int numBytes)
{
    n_assert(this->IsOpen());

    // statistics
    nFileServer2::Instance()->AddBytesWritten(numBytes);

#ifdef __WIN32__
    DWORD written;
    WriteFile(this->handle, buffer, numBytes, &written, NULL);
    return written;
#else
    return fwrite(buffer, 1, numBytes, this->fp);
#endif
}

//------------------------------------------------------------------------------
/**
    reads a number of bytes from the file

    @param buffer          buffer for data
    @param numBytes        number of bytes to read
    @return                number of bytes read

    history:
     - 30-Jan-2002   peter    created
*/
int
nFile::Read(void* buffer, int numBytes)
{
    n_assert(this->IsOpen());

    // statistics
    nFileServer2::Instance()->AddBytesRead(numBytes);

#ifdef __WIN32__
    DWORD read;
    ReadFile(this->handle, buffer, numBytes, &read, NULL);
    return read;
#else
    return fread(buffer, 1, numBytes, this->fp);
#endif
}


//------------------------------------------------------------------------------
/**
    gets current position of file pointer

    @return          position of pointer

    history:
     - 30-Jan-2002   peter    created
*/
int
nFile::Tell() const
{
    n_assert(this->IsOpen());
#ifdef __WIN32__
    return SetFilePointer(this->handle, 0, NULL, FILE_CURRENT);
#else
    return ftell(this->fp);
#endif
}

//------------------------------------------------------------------------------
/**
    sets the file pointer to given absolute or relative position

    @param byteOffset        the offset
    @param origin            position from which to count
    @return                  success

    history:
     - 30-Jan-2002   peter    created
*/
bool
nFile::Seek(int byteOffset, nSeekType origin)
{
    n_assert(this->IsOpen());

    nFileServer2::Instance()->AddSeek();

#ifdef __WIN32__
    DWORD method;
    switch (origin)
    {
        case CURRENT:
            method = FILE_CURRENT;
            break;
        case START:
            method = FILE_BEGIN;
            break;
        case END:
            method = FILE_END;
            break;
    }

    DWORD ret = SetFilePointer(this->handle, (LONG)byteOffset, NULL, method);
    return (ret != 0xffffffff);
#else
    int whence = SEEK_SET;
    switch (origin)
    {
        case CURRENT:
            whence = SEEK_CUR;
            break;
        case START:
            whence = SEEK_SET;
            break;
        case END:
            whence = SEEK_END;
            break;
    }
    return (0 == fseek(this->fp, byteOffset, whence)) ? true : false;
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nFile::Eof() const
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    DWORD fpos = SetFilePointer(this->handle, 0, NULL, FILE_CURRENT);
    DWORD size = GetFileSize(this->handle, NULL);

    // NOTE: THE '>=' IS NOT A BUG!!!
    return fpos >= size;
#else
    return feof(this->fp);
#endif
}

//------------------------------------------------------------------------------
/**
    Returns size of file in bytes.

    @return     byte-size of file
*/
int
nFile::GetSize() const
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    return GetFileSize(this->handle, NULL);
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct stat s;
    fstat(fileno(this->fp), &s);
    return s.st_size;
#else
#error "nFile::GetSize(): NOT IMPLEMENTED!"
#endif
}

//------------------------------------------------------------------------------
/**
    Returns time of last write access. The file must be opened in "read" mode
    before this function can be called!
*/
nFileTime
nFile::GetLastWriteTime() const
{
    n_assert(this->IsOpen());

#ifdef __WIN32__
    nFileTime fileTime;
    GetFileTime(this->handle, NULL, NULL, &(fileTime.time));
    return fileTime;
#elif defined(__LINUX__) || defined(__MACOSX__)
    nFileTime fileTime;
    struct stat s;
    fstat(fileno(this->fp), &s);
    fileTime.time = s.st_mtime;
    return fileTime;
#else
#error "nFile::GetLastWriteTime(): NOT IMPLEMENTED!"
#endif
}

//------------------------------------------------------------------------------
/**
    writes a string to the file

    @param buffer        the string to write
    @return              success

    history:
     - 30-Jan-02   peter   created
     - 29-Jan-03   floh    the method suddenly wrote a newLine. WRONG!
*/
bool
nFile::PutS(const nString& buffer)
{
    n_assert(this->IsOpen());

    int len = buffer.Length();
    int written = this->Write(buffer.Get(), len);
    if (written != len)
    {
        return false;
    }
    else
    {
        this->lineNumber++;
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    reads a string from the file up to and including the first newline character
    or up to the end of the buffer

    @param buf            buffer for string
    @param bufSize        maximum number of chars to read
    @return               success (false if eof is reached)

    history:
     - 30-Jan-2002   peter    created
     - 28-Sep-2005   floh     complete rewrite, there were error if bufSize
                              was below a line length
*/
bool
nFile::GetS(char* buf, int bufSize)
{
    n_assert(this->IsOpen());
    n_assert(buf);
    n_assert(bufSize > 1);

    if (this->Eof())
    {
        return false;
    }

    // make room for final terminating 0
    bufSize--;

    // store start filepointer position
    int startPos = this->Tell();

    // read file contents in chunks of 64 bytes, not char by char
    int chunkSize = 256;
    if (chunkSize > bufSize)
    {
        chunkSize = bufSize;
    }
    char* readPos = buf;
    int curIndex;
    for (curIndex = 0; curIndex < bufSize; curIndex++)
    {
        // read next chunk of data?
        if (0 == (curIndex % chunkSize))
        {
            // if we reached end-of-file before, break out
            if (this->Eof())
            {
                break;
            }

            // now, read the next chunk of data
            int readSize = chunkSize;
            if ((curIndex + readSize) >= bufSize)
            {
                readSize = bufSize - curIndex;
            }
            int bytesRead = this->Read(readPos, readSize);
            if (bytesRead != readSize)
            {
                // end of file reached
                readPos[bytesRead] = 0;
            }
            readPos += bytesRead;
        }

        // check for newline
        if (buf[curIndex] == '\n')
        {
            // reset file pointer to position after new-line
            this->Seek(startPos + curIndex + 1, START);
            break;
        }
    }
    buf[curIndex] = 0;
    return true;
}

//------------------------------------------------------------------------------
/**
    Append the contents of another file to this file. This and the 'other' file
    must both be open! Returns number of bytes copied.
    Warning: current implementation reads the complete source file
    into a ram buffer.

    @return     number of bytes appended
*/
int
nFile::AppendFile(nFile* other)
{
    n_assert(other);

    int numBytes = other->GetSize();
    if (numBytes == 0)
    {
        // nothing to do
        return 0;
    }

    // allocate temp buffer and read bytes
    void* buffer = n_malloc(numBytes);
    n_assert(buffer);
    int numBytesRead = other->Read(buffer, numBytes);
    n_assert(numBytesRead == numBytes);

    // write to this file
    int numBytesWritten = this->Write(buffer, numBytes);
    n_assert(numBytesWritten == numBytes);

    // cleanup
    n_free(buffer);
    return numBytes;
}
