#define N_IMPLEMENTS nRamFile
//------------------------------------------------------------------------------
//  nramfile.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "file/nramfile.h"
#include "file/nsharedmemory.h"

const nString nRamFile::SharedMemoryRoot = "/usr/share/memory";

//------------------------------------------------------------------------------
/**
*/
nRamFile::nRamFile() :
    filePointerPos(0),
    fallBack(false)
{
    // empty    
}

//------------------------------------------------------------------------------
/**
*/
nRamFile::~nRamFile()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRamFile::Exists(const nString& fileName) const
{
    return nKernelServer::Instance()->Lookup(GetFullSharedMemoryObjectName(GetBaseName(fileName)).Get()) != 0 ||
           nFile::Exists(fileName);
}

//------------------------------------------------------------------------------
/**
*/
bool
nRamFile::Open(const nString& fileName, const nString& accessMode)
{
    n_assert(fileName.IsValid());
    n_assert(accessMode.IsValid());

    // "Connect" to shared memory object.
    n_assert(!this->refSharedMemory.isvalid());

    // Determine object's name.
    nString sharedMemoryObjectName = GetFullSharedMemoryObjectName(GetBaseName(fileName));

    // Try a lookup.
    this->refSharedMemory = static_cast<nSharedMemory*>(nKernelServer::Instance()->Lookup(sharedMemoryObjectName.Get()));

    // Exists? No - create new one.
    if (!this->refSharedMemory.isvalid())
    {
        this->refSharedMemory = static_cast<nSharedMemory*>(nKernelServer::Instance()->New("nsharedmemory", sharedMemoryObjectName.Get()));
    }
    n_assert(this->refSharedMemory.isvalid());

    //for (uint i = 0; i < strlen(accessMode); i++) 
    //{
    //    if (accessMode[i] == 'w')
    //    {
    //        this->refSharedMemory->Create();
    //        break;
    //    }
    //    else if (accessMode[i] == 'r')
    //    {
    //        this->refSharedMemory->Open();
    //        break;
    //    }
    //}
    if (accessMode.ContainsCharFromSet("rR"))
    {
        this->refSharedMemory->Open();
    }
    if (accessMode.ContainsCharFromSet("wW"))
    {
        this->refSharedMemory->Create();
    }

    this->isOpen = this->refSharedMemory->IsOpen();

    // Fall back to nFile?
    if (this->isOpen)
    {
        this->fallBack = false;
        this->filePointerPos = 0;
    }
    else
    {
        this->fallBack = true;
        this->refSharedMemory->Release();
        this->refSharedMemory = 0;
        return nFile::Open(fileName, accessMode);
    }

    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
void
nRamFile::Close()
{
    n_assert(IsOpen());
    if (this->refSharedMemory.isvalid() && this->refSharedMemory->IsOpen())
    {
        this->refSharedMemory->Close();
        this->refSharedMemory = 0;
    }
    this->isOpen = false;
    this->fallBack = false;
    this->filePointerPos = 0;
}

//------------------------------------------------------------------------------
/**
*/
int
nRamFile::Read(void* buffer, int numBytes)
{
    n_assert(this->IsOpen());
    n_assert(buffer != 0);
    n_assert(numBytes > 0);

    // Fall back mode?
    if (this->fallBack)
    {
        return nFile::Read(buffer, numBytes);
    }

    // End of file?
    if (Eof())
    {
        return 0;
    }

    // Adjust temporary buffer if necessary.
    if (this->refSharedMemory->ReadBufferCapacity() < numBytes)
    {
        this->refSharedMemory->ResizeReadBuffer(numBytes);
    }

    // Determine start- and end index in memory.
    int startIndex = this->filePointerPos;
    int endIndex = startIndex + numBytes - 1;
    if (!this->refSharedMemory->Readable(startIndex, endIndex))
    {
        endIndex = this->refSharedMemory->Count() - 1;
    }

    // Read data.
    int bytesRead = endIndex - startIndex + 1;
    this->refSharedMemory->Read(startIndex, endIndex);
    this->filePointerPos += bytesRead;

    // Copy into target buffer.
    memcpy(buffer, this->refSharedMemory->LastRead(), bytesRead);

    return bytesRead;
}

//------------------------------------------------------------------------------
/**
    22-Nov-04   floh    writing 0 bytes is not an error
*/
int
nRamFile::Write(const void* buffer, int numBytes)
{
    n_assert(this->IsOpen());
    n_assert(buffer != 0);

    if (0 == numBytes)
    {
        return 0;
    }

    // Fall back mode?
    if (this->fallBack)
    {
        return nFile::Write(buffer, numBytes);
    }

    // Resize buffer if necessary.
    int startIndex = this->filePointerPos;
    int endIndex = startIndex + numBytes - 1;
    while (!this->refSharedMemory->Writable(startIndex, endIndex))
    {
        this->refSharedMemory->Resize(this->refSharedMemory->Capacity() * 2);
    }
    // Write data.
    this->refSharedMemory->Write((const char*)buffer, startIndex, endIndex);
    this->filePointerPos += numBytes;

    return numBytes;
}

//------------------------------------------------------------------------------
/**
*/
int
nRamFile::Tell() const
{
    if (this->fallBack)
    {
        return nFile::Tell();
    }
    return this->filePointerPos;
}

//------------------------------------------------------------------------------
/**
*/
bool
nRamFile::Seek(int byteOffset, nSeekType origin)
{
    // Fall back mode.
    if (this->fallBack)
    {
        return nFile::Seek(byteOffset, origin);
    }

    switch (origin)
    {
    case START:
        this->filePointerPos = byteOffset;
        break;

    case END:
        this->filePointerPos = this->refSharedMemory->Count() - byteOffset;
        break;

    case CURRENT:
        this->filePointerPos = this->filePointerPos + byteOffset;
        break;
    }

    // Keep position in bounds.
    if (this->filePointerPos < 0)
    {
        this->filePointerPos = 0;
    }
    if (this->filePointerPos > this->refSharedMemory->Count())
    {
        this->filePointerPos = this->refSharedMemory->Count();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
int
nRamFile::GetSize() const
{
    n_assert(this->IsOpen());

    if (this->fallBack)
    {
        return nFile::GetSize();
    }
    return this->refSharedMemory->Count();
}

//------------------------------------------------------------------------------
/**
*/
bool
nRamFile::Eof() const
{
    n_assert(this->IsOpen());
    if (this->fallBack)
    {
        return nFile::Eof();
    }
    return this->filePointerPos >= refSharedMemory->Count();
}

//------------------------------------------------------------------------------
/**
*/
nFileTime
nRamFile::GetLastWriteTime() const
{
    // Fall back mode?
    if (this->fallBack)
    {
        return nFile::GetLastWriteTime();
    }
    // Dummy file time.
    nFileTime fileTime;
    return fileTime;
}

//------------------------------------------------------------------------------
/**
*/
nString
nRamFile::GetFullSharedMemoryObjectName(const nString& n) const
{
    return nString(SharedMemoryRoot + "/" + n);
}

//------------------------------------------------------------------------------
/**
*/
nString
nRamFile::GetBaseName(const nString& n) const
{
    nString result = n.ExtractFileName();
//    result.StripExtension();
    return result;
}    

//------------------------------------------------------------------------------
