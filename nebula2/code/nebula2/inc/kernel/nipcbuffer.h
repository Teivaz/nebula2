#ifndef N_IPCBUFFER_H
#define N_IPCBUFFER_H
//------------------------------------------------------------------------------
/**
    @class nIpcBuffer
    @ingroup Ipc

    @brief A message buffer for the nIpc* class family.

    Note that a buffer is just a raw byte pool, especially buffers used
    for receiving messages may contain more then one message!

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nIpcBuffer
{
public:
    /// constructor
    nIpcBuffer(int maxSize);
    /// constructor 2
    nIpcBuffer(const char* buf, int numBytes);
    /// constructor with string argument
    nIpcBuffer(const char* str);
    /// destructor
    ~nIpcBuffer();
    /// assignment operator
    nIpcBuffer& operator=(const nIpcBuffer& rhs);
    /// get max size
    int GetMaxSize() const;
    /// set buffer content
    int Set(const char* buf, int numBytes);
    /// get pointer to buffer content
    char* GetPointer();
    /// get const pointer to buffer content
    const char* GetPointer() const;
    /// set size of buffer content manually
    void SetSize(int s);
    /// get size of buffer content
    int GetSize() const;
    /// set buffer content as string
    int SetString(const char* str);
    /// get (first) string in buffer
    const char* GetString() const;
    /// check if the buffer content is a valid string
    bool IsValidString() const;
    /// get first string in a multi-string buffer
    const char* GetFirstString();
    /// get the next string in a multi-string buffer
    const char* GetNextString();

private:
    int bufferSize;
    int contentSize;
    char* buffer;
    const char* curStringPointer;
};

//------------------------------------------------------------------------------
/**
    Sets buffer contents. Returns number of bytes copied (this may be less
    then intended if a buffer overflow would occur.
*/
inline
int
nIpcBuffer::Set(const char* buf, int numBytes)
{
    n_assert(buf);
    if (numBytes > this->bufferSize)
    {
        numBytes = this->bufferSize;
    }
    memcpy(this->buffer, buf, numBytes);
    this->contentSize = numBytes;
    return numBytes;
}

//------------------------------------------------------------------------------
/**
    Set the buffer content as string. The string may be truncated if it
    is too long. Returns the number of bytes copied (includes the
    terminating 0).
*/
inline
int
nIpcBuffer::SetString(const char* str)
{
    n_assert(str);
    n_strncpy2(this->buffer, str, this->bufferSize);
    this->contentSize = strlen(this->buffer) + 1;
    return this->contentSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpcBuffer&
nIpcBuffer::operator=(const nIpcBuffer& rhs)
{
    this->Set(rhs.buffer, rhs.contentSize);
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpcBuffer::nIpcBuffer(int maxSize) :
    bufferSize(maxSize),
    contentSize(0),
    curStringPointer(0)
{
    this->buffer = (char*) n_malloc(maxSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpcBuffer::nIpcBuffer(const char* buf, int numBytes) :
    bufferSize(numBytes),
    curStringPointer(0)
{
    this->buffer = (char*) n_malloc(numBytes);
    this->Set(buf, numBytes);
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpcBuffer::nIpcBuffer(const char* buf) :
    curStringPointer(0)
{
    n_assert(buf);
    this->bufferSize = strlen(buf) + 1;
    this->buffer = (char*) n_malloc(this->bufferSize);
    this->SetString(buf);
}

//------------------------------------------------------------------------------
/**
*/
inline
nIpcBuffer::~nIpcBuffer()
{
    n_assert(this->buffer);
    n_free(this->buffer);
    this->buffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nIpcBuffer::GetMaxSize() const
{
    return this->bufferSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
char*
nIpcBuffer::GetPointer()
{
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nIpcBuffer::GetPointer() const
{
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nIpcBuffer::SetSize(int s)
{
    n_assert((s >= 0) && (s <= this->bufferSize));
    this->contentSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nIpcBuffer::GetSize() const
{
    return this->contentSize;
}

//------------------------------------------------------------------------------
/**
    Check if the buffer content is a valid string. This is the case
    if the last content byte is a 0.
*/
inline
bool
nIpcBuffer::IsValidString() const
{
    return ((this->contentSize > 0) && (0 == this->buffer[this->contentSize - 1]));
}

//------------------------------------------------------------------------------
/**
    Get the buffer content as string. Should only be used if the
    buffer content has been set with the SetString() method before.
    If in doubt, use the IsValidString() method to check this out.
*/
inline
const char*
nIpcBuffer::GetString() const
{
    n_assert(this->IsValidString());
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
    Returns the first string in a buffer containing multiple strings. Returns
    0 if buffer content is not a valid string.
*/
inline
const char*
nIpcBuffer::GetFirstString()
{
    n_assert(this->buffer);
    if (this->IsValidString())
    {
        this->curStringPointer = this->buffer;
        return this->curStringPointer;
    }
    else
    {
        this->curStringPointer = 0;
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the next string in a buffer containing multiple strings, or 0
    if no more strings in buffer.
*/
inline
const char*
nIpcBuffer::GetNextString()
{
    n_assert(this->curStringPointer);
    n_assert(this->buffer);
    const char* nextString = this->curStringPointer + strlen(this->curStringPointer) + 1;
    if (nextString >= (this->buffer + this->contentSize))
    {
        return 0;
    }
    else
    {
        this->curStringPointer = nextString;
        return this->curStringPointer;
    }
}

//------------------------------------------------------------------------------
#endif
