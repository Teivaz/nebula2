//------------------------------------------------------------------------------
/**
    nbuffer.cc
*/
#include <memory.h>
#include "kernel/ntypes.h"
#include "util/nbuffer.h"


// Initial buffer size.
const int nBuffer::InitialCapacity = 4096;

//------------------------------------------------------------------------------
/**
*/
nBuffer::nBuffer() :
    capacity(InitialCapacity),
    count(0)
{
    this->data = this->MakeArea(InitialCapacity);
}

//------------------------------------------------------------------------------
/**
*/
nBuffer::nBuffer(const nBuffer& other) :
    capacity(other.capacity),
    count(other.count)
{
    this->data = this->MakeArea(capacity);
    this->Copy(this->data, other.data, other.count);
}

//------------------------------------------------------------------------------
/**
*/
nBuffer::~nBuffer()
{
    delete [] this->data;
}

//------------------------------------------------------------------------------
/**
*/
void
nBuffer::Reset()
{
    delete [] this->data;
    this->data = this->MakeArea(InitialCapacity);
    this->capacity = InitialCapacity;
    this->count = 0;
}

//------------------------------------------------------------------------------
/**
    Do not lose any previously entered items.
*/
void
nBuffer::Resize(int newSize)
{
    n_assert(newSize >= 0);

    if (newSize > this->capacity)
    {
        char* newData = this->MakeArea(newSize);
        if (this->count > 0)
        {
            this->Copy(newData, this->data, this->count);
        }
        delete [] this->data;
        this->data = newData;
        this->capacity = newSize;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nBuffer::Write(const char* data, int startIndex, int endIndex)
{
    n_assert(data != 0);
    n_assert(this->ValidRange(startIndex, endIndex));

    char* p = this->data;
    p += startIndex;
    this->Copy(p, data, endIndex - startIndex + 1);
    if (this->count <= endIndex)
    {
        this->count = endIndex + 1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nBuffer::Read(char* data, int startIndex, int endIndex)
{
    n_assert(data != 0);
    n_assert(this->ValidRange(startIndex, endIndex));

    char* p = this->data;
    p += startIndex;
    this->Copy(data, p, endIndex - startIndex + 1);
}

//------------------------------------------------------------------------------
/**
*/
bool
nBuffer::ValidIndex(int v) const
{
    return 0 <= v && v <= this->Capacity() - 1;
}

//------------------------------------------------------------------------------
/**
*/
bool
nBuffer::ValidRange(int startIndex, int endIndex) const
{
    return this->ValidIndex(startIndex) && this->ValidIndex(endIndex) && endIndex >= startIndex;
}

//------------------------------------------------------------------------------
/**
*/
nBuffer&
nBuffer::operator = (const nBuffer& other)
{
    if (&other == this)
    {
        return *this;
    }

    this->Resize(other.capacity);
    this->Copy(this->data, other.data, other.count);
    count = other.count;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
void
nBuffer::Copy(char* a, const char* b, int size) const
{
    n_assert(a != 0);
    n_assert(b != 0);
    n_assert(size > 0);

    memcpy(a, b, size);
}

//------------------------------------------------------------------------------
/**
*/
char*
nBuffer::MakeArea(int cap) const
{
    n_assert(cap > 0);
    char* result = new char[cap];
    n_assert(result != 0);
    return result;
}

