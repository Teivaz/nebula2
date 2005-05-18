#define N_IMPLEMENTS nSharedMemory
//------------------------------------------------------------------------------
//  nsharedmemory.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "file/nsharedmemory.h"

nNebulaClass(nSharedMemory, "nroot");

/// Initialize static members.
const int nSharedMemory::InitialReadBufferCapacity = 1024;

/// Header size in bytes.
const int nSharedMemory::HeaderSize = 9;


//------------------------------------------------------------------------------
/**
*/
nSharedMemory::nSharedMemory() :
    readBuffer(0),
    readBufferCapacity(0),
    capacity(0),
    count(0),
    mapHandle(0),
    mapHeader(0),
    mapBody(0),
    isOpen(false),
    writable(false)
{
    SetupReadBuffer(InitialReadBufferCapacity);
}

//------------------------------------------------------------------------------
/**
*/
nSharedMemory::~nSharedMemory()
{
    n_assert(!IsOpen());

    if (this->mapHandle != 0)
    {
        DestroyMapping();
    }
    delete [] this->readBuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Create()
{
    n_assert(!this->IsOpen());

    if (this->mapHandle != 0)
    {
        DestroyMapping();
    }
    this->writeBuffer.Reset();
    this->isOpen = true;
    this->writable = true;
    this->capacity = this->writeBuffer.Capacity();
    this->count = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Resize(int n)
{
    n_assert(n > 0);
    n_assert(this->Writable(0, this->Capacity() - 1));

    this->writeBuffer.Resize(n);
    this->capacity = this->writeBuffer.Capacity();
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Open()
{
    n_assert(!this->IsOpen());

    // Open existing file mapping.
    this->mapHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,  // Read+write access.
                                        FALSE,                // Don't inherit handle.
                                        GetName());

    if (this->mapHandle == NULL)
    {
        n_printf("Failed to open file mapping (Error: %d).\n", GetLastError());
        return;
    }

    // Map view of file.
    this->mapHeader = MapViewOfFile(this->mapHandle,       // Map handle.
                                    FILE_MAP_ALL_ACCESS,   // Read+write access.
                                    0,
                                    0,
                                    0);

    if (this->mapHeader == NULL)
    {
        CloseHandle(this->mapHandle);
        n_printf("Failed to map view of file (Error: %d).\n", GetLastError());
        return;
    }

    this->mapBody = (char*)this->mapHeader;
    this->mapBody += HeaderSize;
    this->isOpen = true;
    this->writable = false;
    this->LockMemory();
    this->ReadHeader();
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Close()
{
    n_assert(IsOpen());

    if (this->writable)
    {
        n_assert(this->mapHandle == 0);

        // Create mapping.
        this->mapHandle = CreateFileMapping((HANDLE)0xffffffff,  // Page file.
                                             NULL,               // Handle cannot be inherited.
                                             PAGE_READWRITE,     // Read+write access.
                                             0,                  // ??
                                             this->Count(),      // Size in bytes.
                                             GetName());         // Mapping's name.

        if (this->mapHandle == NULL)
        {
            n_printf("Failed to create file mapping (Error: %d).\n", GetLastError());
            n_assert(false);
            return;
        }

        // Create view of file in current process's address space.
        this->mapHeader = MapViewOfFile(this->mapHandle,       // Map handle.
                                        FILE_MAP_ALL_ACCESS,   // Read+write access.
                                        0,                     // ?
                                        0,                     // ?
                                        0);                    // Map entire file.

        if (this->mapHeader == NULL)
        {
            CloseHandle(this->mapHandle);
            n_printf("Failed to map view of file (Error: %d).\n", GetLastError());
            n_assert(false);
            return;
        }

        this->mapBody = (char*)this->mapHeader;
        this->mapBody += HeaderSize;

        this->LockMemory();
        this->WriteHeader();
        this->WriteBody();
        this->UnlockMemory();
    }
    else
    {
        n_assert(this->mapHandle != 0);
        this->UnlockMemory();
        this->DestroyMapping();
    }

    this->isOpen = false;
    this->writable = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSharedMemory::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
int
nSharedMemory::Capacity() const
{
    return this->capacity;
}

//------------------------------------------------------------------------------
/**
*/
int
nSharedMemory::Count() const
{
    return this->count;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::ResizeReadBuffer(int n)
{
    n_assert(n > 0);
    delete [] this->readBuffer;
    this->readBuffer = 0;
    SetupReadBuffer(n);
}

//------------------------------------------------------------------------------
/**
*/
int
nSharedMemory::ReadBufferCapacity() const
{
    return this->readBufferCapacity;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Write(const char* v, int start, int end)
{
    n_assert(this->Writable(start, end));
    n_assert(v != 0);

    this->writeBuffer.Write(v, start, end);
    if (this->count <= end)
    {
        this->count = end + 1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::Read(int start, int end)
{
    n_assert(this->Readable(start, end));
    n_assert(ReadBufferCapacity() >= end - start + 1);

    if (this->writable)
    {
        this->writeBuffer.Read(this->readBuffer, start, end);
    }
    else
    {
        char* address = this->mapBody;
        address += start;
        memcpy(this->readBuffer, address, end - start + 1);
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nSharedMemory::LastRead() const
{
    n_assert(this->readBuffer != 0);
    return this->readBuffer;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSharedMemory::Readable(int start, int end) const
{
    return this->IsOpen() &&
           (0 <= start && start <= Count() - 1) &&
           (0 <= end && end <= Count() - 1) &&
           (end >= start);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSharedMemory::Writable(int start, int end) const
{
    return this->IsOpen() &&
           this->writable &&
           (0 <= start && start <= Capacity() - 1) &&
           (0 <= end && end <= Capacity() - 1) &&
           (end >= start);
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::SetupReadBuffer(int n)
{
    n_assert(this->readBuffer == 0);
    this->readBuffer = new char[n];
    this->readBufferCapacity = n;
    n_assert(this->readBuffer != 0);
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::DestroyMapping()
{
    // Unmap view of file.
    if (!UnmapViewOfFile(this->mapHeader))
    {
        n_printf("Failed to unmap view of file (Error: %d).\n", GetLastError());
        return;
    }

    // Close map handle.
    if (!CloseHandle(this->mapHandle))
    {
        n_printf("Failed to close map handle. (Error: %d)\n", GetLastError());
        return;
    }

    this->capacity = 0;
    this->count = 0;
    this->mapBody = 0;
    this->mapHeader = 0;
    this->mapHandle = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::ReadHeader()
{
    n_assert(this->IsOpen());
    n_assert(this->mapHeader != 0);

    char* p = (char*)this->mapHeader;

    // 1 Byte = `lock'.
    p += 1;

    // 4 Byte = `capacity'.
    memcpy(&this->capacity, p, 4);
    p += 4;

    // 4 Byte = `count'.
    memcpy(&this->count, p, 4);
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::WriteHeader()
{
    n_assert(this->IsOpen());
    n_assert(this->mapHeader != 0);

    char* p = (char*)this->mapHeader;

    // 1 Byte = `lock'.
    p += 1;

    // 4 Byte = `capacity'.
    memcpy(p, &this->capacity, 4);
    p += 4;

    // 4 Byte = `count'.
    memcpy(p, &this->count, 4);
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::WriteBody()
{
    n_assert(this->mapBody != 0);

    memcpy(this->mapBody, this->writeBuffer.Get(), this->writeBuffer.Count());
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::LockMemory()
{
    n_assert(this->IsOpen());
    n_assert(this->mapHeader != 0);

    char* p = (char*)this->mapHeader;
    while (*p != 0);  // Wait for unlock.
    *p = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nSharedMemory::UnlockMemory()
{
    n_assert(this->IsOpen());
    n_assert(this->mapHeader != 0);

    char* p = (char*)this->mapHeader;
    *p = 0;
}

//------------------------------------------------------------------------------
