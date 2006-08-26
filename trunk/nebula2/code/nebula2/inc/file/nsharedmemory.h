#ifndef N_SHAREDMEMORY_H
//------------------------------------------------------------------------------
/**
    @class nSharedMemory
    @ingroup RAMFile

    Memory that can be used by two or more processes for IPC.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/nbuffer.h"
#if defined(__LINUX__) || defined(__MACOSX__)
#include "util/nstring.h"
#endif

//------------------------------------------------------------------------------
class nSharedMemory : public nRoot
{
public:
    /// constructor
    nSharedMemory();
    /// destructor
    virtual ~nSharedMemory();

    /// Create in read+write access mode.
    void Create();
    /// Resize to accommodate `n' items. Do not lose any previously entered items.
    void Resize(int n);
    /// Open existing memory block in read only access mode.
    void Open();
    /// Close memory block.
    void Close();
    /// Is memory block usable?
    bool IsOpen() const;

    /// Number of bytes available in memory.
    int Capacity() const;
    /// Number of bytes written so far.
    int Count() const;

    /// Resize temporary buffer to accommodate `n' bytes.
    void ResizeReadBuffer(int n);
    /// Number of bytes available in buffer.
    int ReadBufferCapacity() const;

    /// Write contents of `v' into memory starting from index `start' until index `end'.
    void Write(const char* v, int start, int end);
    /// Read contents of memory starting from index `start' until index `end' into
    /// temporary buffer. Make result available in `LastRead'.
    void Read(int start, int end);
    /// Result of last call to `Read'.
    const char* LastRead() const;

    /// Is range from `start' to `end' writable?
    bool Writable(int start, int end) const;
    /// Is range from `start' to `end' readable?
    bool Readable(int start, int end) const;

    /// Initial temporary buffer size.
    static const int InitialReadBufferCapacity;
    /// Size of header in bytes.
    static const int HeaderSize;

private:
    // Create temporary read buffer with capacity of `n' bytes.
    void SetupReadBuffer(int n);
    // Destroy file mapping.
    void DestroyMapping();
    // Read information about memory contents from header of memory block.
    void ReadHeader();
    // Write information about memory contents in header of memory block.
    void WriteHeader();
    // Copy contents of `v' into shared memory body.
    void WriteBody();
    // Lock memory contents for exclusive use.
    void LockMemory();
    // Unlock memory contents.
    void UnlockMemory();

    // Data
    nBuffer writeBuffer;
    char* readBuffer;
    int readBufferCapacity;
    int capacity;
    int count;
#ifdef __WIN32__
    HANDLE mapHandle;
#elif defined(__LINUX__) || defined(__MACOSX__)
    int mapHandle;
    nString mapFileName;
#endif
    void* mapHeader;
    char* mapBody;
    bool isOpen;
    bool writable;
};
//------------------------------------------------------------------------------
#endif
