#ifndef N_BUFFER_H
#define N_BUFFER_H
//------------------------------------------------------------------------------
/**
    @class nBuffer
    @ingroup util

    Simple byte buffers.
*/
#include "kernel/ntypes.h"


class nBuffer
{
public:
    /// Construct.
    nBuffer();
    /// Construct from buffer.
    nBuffer(const nBuffer& other);
    /// Destruct.
    ~nBuffer();

    /// Initial buffer size.
    static const int InitialCapacity;

    /// Reset to initial state.
    void Reset();
    /// Resize buffer to accommodate `newSize' bytes.
    void Resize(int newSize);
    /// Maximum number of bytes available.
    int Capacity() const;
    /// Number of items available.
    int Count() const;
    /// Access raw buffer.
    const char* Get() const;

    /// Write `data' starting from `startIndex' to `endIndex'.
    void Write(const char* data, int startIndex, int endIndex);
    /// Read bytes from `startIndex' to `endIndex'.
    void Read(char* data, int startIndex, int endIndex);

    /// Is `v' a valid index?
    bool ValidIndex(int v) const;
    /// Is `startIndex' to `endIndex' a valid range?
    bool ValidRange(int startIndex, int endIndex) const;

    /// Copy contents of `other' to `*this'.
    nBuffer& operator = (const nBuffer& other);

private:
    /// Copy contents of `b' to `a'.
    void Copy(char* a, const char* b, int size) const;
    /// New memory block with capacity `cap'.
    char* MakeArea(int cap) const;

    // Data
    char* data;
    int capacity;
    int count;
};

//------------------------------------------------------------------------------
inline
int
nBuffer::Capacity() const
{
    n_assert(capacity > 0);
    return capacity;
}
//------------------------------------------------------------------------------
inline
int
nBuffer::Count() const
{
    n_assert(0 <= count && count <= capacity);
    return count;
}
//------------------------------------------------------------------------------
inline
const char*
nBuffer::Get() const
{
    n_assert(data != 0);
    return data;
}
//------------------------------------------------------------------------------
#endif
