#ifndef N_BITSTREAM_H
#define N_BITSTREAM_H
//------------------------------------------------------------------------------
/**
    @class nBitStream
    @ingroup NebulaDataTypes

    @brief A bitstream codec class.

    (C) 2000 Radon Labs GmbH
*/
#include <memory.h>
#include "kernel/ntypes.h"
#include "util/nnode.h"
#include "mathlib/nmath.h"

//------------------------------------------------------------------------------
class nBitStream : public nNode
{
public:
    /// standard constructor, requires separate SetSize()
    nBitStream();
    /// constructor, expects stream size in bytes
    nBitStream(int size);
    /// copy constructor
    nBitStream(const nBitStream& other);
    /// the destructor
    virtual ~nBitStream();

    /// directly set contents of stream
    void Set(const uchar* s, int size);
    /// directly get contents of stream
    const uchar* Get() const;
    /// Set size of stream.
    void SetSize(int size);
    /// get the byte size of the stream
    int GetSize() const;
    /// set the current bit position in the stream
    void SetPos(int pos);
    /// return the current bit position in the stream
    int GetPos() const;
    /// return the number of bits left in the stream
    int BitsLeft() const;

    /// write a bool to the stream
    void WriteBool(bool flag);
    /// read a bool from the stream
    bool ReadBool();
    /// write a compressed integer to the stream
    void WriteInt(int value, int numBits);
    /// read a compressed integer from the stream
    int ReadInt(int numBits);
    /// write a float to the stream
    void WriteFloat(float value);
    /// read a float from the stream
    float ReadFloat();
    /// write a custom byte array to the stream
    void WriteCustom(const void* ptr, int size);
    /// read a custom byte array from the stream
    void ReadCustom(void* ptr, int size);
    /// write bitstream
    void WriteBitStream(nBitStream& stream);
    /// read bitstream
    void ReadBitStream(nBitStream& stream);
    /// copy data from source stream into this stream
    void Copy(const nBitStream& source);

    /// Lock for writing.
    void BeginWrite();
    /// Unlock for writing.
    void EndWrite();
    /// Lock for reading.
    void BeginRead();
    /// Unlock for reading.
    void EndRead();
    /// Writable?
    bool IsWritable() const;
    /// Readable?
    bool IsReadable() const;

    /// Assignment operator.
    nBitStream& operator = (const nBitStream& s);
    /// Equality operator.
    bool operator == (const nBitStream& s) const;
    /// Inequality operator.
    bool operator != (const nBitStream& s) const;

    // output
    void Print(int lines);

protected:
    /// clear stream contents, do not rewind bit pointer
    void Clear();
    /// Write bit at current position to stream.
    void WriteBit(bool bit);
    /// Read bit at currernt position from stream.
    bool ReadBit();

private:
    unsigned char* stream;
    int streamSize;
    int currentByte;    /// Current index in the stream buffer.
    int currentBit;     /// Current bit offset int currentByte.
    bool writable;
    bool readable;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBitStream::nBitStream()
{
    streamSize  = 0;
    currentByte = 0;
    currentBit  = 0;
    writable    = false;
    readable    = false;
    stream      = 0;
}

//------------------------------------------------------------------------------
/**
    @param  size    the byte size of the bit stream (rounded up)
*/
inline
nBitStream::nBitStream(int size)
{
    // require
    n_assert(size > 0);

    streamSize  = size;
    currentByte = 0;
    currentBit  = 0;
    writable   = false;
    readable    = false;

    stream = n_new_array(unsigned char, size);
    n_assert(stream != 0);
    memset(stream, 0, streamSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
nBitStream::nBitStream(const nBitStream& other) :
    streamSize(other.streamSize),
    currentByte(0),
    currentBit(0),
    writable(false),
    readable(false)
{
    stream = n_new_array(unsigned char, streamSize);
    n_assert(stream != 0);
    Copy(other);
}

//------------------------------------------------------------------------------
/**
*/
inline
nBitStream::~nBitStream()
{
    if (stream != 0)
    {
        n_delete_array(stream);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::Clear()
{
    // require
    n_assert(stream != 0);
    n_assert(streamSize > 0);

    memset(stream, 0, streamSize);
}

//------------------------------------------------------------------------------
/**
    Write a bool value (always encoded into 1 bit) to the current
    stream position.

    @param  flag    the bool value
*/
inline
void
nBitStream::WriteBool(bool flag)
{
    n_assert(writable);

    WriteBit(flag);
}

//------------------------------------------------------------------------------
/**
    Reads a bool value (always encoded into 1 bit) from the current
    stream position.

    @return     the bool value
*/
inline
bool
nBitStream::ReadBool()
{
    n_assert(readable);

    return ReadBit();
}

//------------------------------------------------------------------------------
/**
    Write a signed integer clamped to numBits bits.

    @param  value       the integer value to write
    @param  numBits     number of bits to write
*/
inline
void
nBitStream::WriteInt(int value, int numBits)
{
    n_assert(writable);
    n_assert((1 < numBits) && (numBits <= 32));
    n_assert(BitsLeft() >= numBits);

    // Write sign
    int tmp = (1 << 31);
    int tmp2 = (value & tmp);
    WriteBit(tmp2 != 0);
    numBits -= 2;       // I use numBits for shifting
                        // 1 Bit less for sign
                        // 1 Bit less for shifting

    // Make absolute value;
    // write sign and value separate
    value = n_abs(value);

    // Write value
    while (numBits >= 0)
    {
        tmp = (1 << numBits);
        tmp2 = (value & tmp);
        WriteBit(tmp2 != 0);
        numBits--;
    }

    n_assert(numBits == -1);
}

//------------------------------------------------------------------------------
/**
    Read a signed integer clamped to numBits bits, the rest of the
    32 bytes will be filled with the correct sign bit.

    @param  numBits     number of bits to read
    @return             the integer, correclty expanded to 32 bits
*/
inline
int
nBitStream::ReadInt(int numBits)
{
    n_assert(readable);
    n_assert((1 < numBits) && (numBits <= 32));
    n_assert(BitsLeft() >= numBits);

    int value = 0;

    // Read sign
    bool sign = ReadBit();
    numBits -= 2;       // I use numBits for shifting
                        // 1 Bit less for sign
                        // 1 Bit less for shifting

    // Read data
    while (numBits >= 0)
    {
        int v = ReadBit() ? 1 : 0;
        value |= (v << numBits);
        numBits--;
    }

    n_assert(numBits == -1);

    // set sign
    if (sign)
    {
        value = -value;
    }

    return value;
}

//------------------------------------------------------------------------------
/**
    Write a float value into the stream, the float value will always be
    encoded into 32 bits.

    @param  value   the float value to write
*/
inline
void
nBitStream::WriteFloat(float value)
{
    n_assert(writable);
    n_assert(BitsLeft() >= 32);

    int tmp;
    memcpy(&tmp, &value, 4);
    WriteInt(tmp, 32);      // Write floats as ints; use 32 BITS
}

//------------------------------------------------------------------------------
/**
    Read a float value (encoded in 32 bit) from the stream.

    @return      the float value
*/
inline
float
nBitStream::ReadFloat()
{
    n_assert(readable);
    n_assert(BitsLeft() >= 32);

    int tmp = ReadInt(32);      // Read floats as int; use 32 BITS
    float value;
    memcpy(&value, &tmp, 4);

    return value;
}

//------------------------------------------------------------------------------
/**
    Write a custom byte array to the stream.

    @param  ptr     pointer to byte array to write to the stream
    @param  size    number of bytes to write
*/
inline
void
nBitStream::WriteCustom(const void* ptr, int size)
{
    n_assert(writable);
    n_assert(ptr != 0);
    n_assert(size > 0);
    n_assert(BitsLeft() >= size * 8);

    // advance to next byte in stream
    // if current bit offset is greater 0
    if (currentBit > 0)
    {
        currentByte++;
        currentBit = 0;
    }
    memcpy(&stream[currentByte], ptr, size);
    currentByte += size;
}

//------------------------------------------------------------------------------
/**
    Read a custom byte array from the stream.

    @param  ptr     pointer to byte array
    @param  size    size in bytes to read
*/
inline
void
nBitStream::ReadCustom(void* ptr, int size)
{
    n_assert(readable);
    n_assert(ptr != 0);
    n_assert(size > 0);
    n_assert(BitsLeft() >= size * 8);

    // advance to next byte in stream
    // if current bit offset is greater 0
    if (currentBit > 0)
    {
        currentByte++;
        currentBit = 0;
    }
    memcpy(ptr, &stream[currentByte], size);
    currentByte += size;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::ReadBitStream(nBitStream& stream)
{
    n_assert(stream.streamSize <= (BitsLeft() * 8));

    ReadCustom(stream.stream, stream.streamSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::WriteBitStream(nBitStream& stream)
{
    n_assert(stream.streamSize <= (BitsLeft() * 8));

    WriteCustom(stream.stream, stream.streamSize);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::Set(const uchar* s, int size)
{
    // require
    n_assert(s != 0);
    n_assert(size > 0);

    // delete old stream, if exits
    if (stream != 0) n_delete_array(stream);

    // create and copy stream
    stream = n_new_array(unsigned char, size);
    n_assert(stream != 0);
    streamSize = size;
    memcpy(stream, s, size);
}

//------------------------------------------------------------------------------
/**
*/
inline
const uchar*
nBitStream::Get() const
{
    return stream;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::SetSize(int size)
{
    n_assert(size > 0);

    if (stream != 0)
    {
        n_delete(stream);
    }
    stream = n_new_array(unsigned char, size);
    n_assert(stream != 0);
    streamSize = size;
    Clear();

    n_assert(streamSize == size);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBitStream::GetSize() const
{
    return streamSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::SetPos(int pos)
{
    n_assert(!writable);       // Can't change bit pos while writing!!!
    n_assert((0 <= pos) && (pos <= streamSize * 8));

    currentByte = pos / 8;
    currentBit  = pos % 8;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBitStream::GetPos() const
{
    return (currentByte * 8) + currentBit;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBitStream::BitsLeft() const
{
    return (GetSize() * 8) - GetPos();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::Copy(const nBitStream& source)
{
    n_assert(source.streamSize > 0);
    n_assert(source.streamSize == streamSize);
    n_assert(stream != 0);
    n_assert(source.stream != 0);

    memcpy(stream, source.stream, source.streamSize);
    currentBit = streamSize * 8;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::BeginWrite()
{
    n_assert(!writable);
    n_assert(!readable);
    Clear();
    SetPos(0);
    writable = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::EndWrite()
{
    n_assert(writable);
    n_assert(!readable);
    writable = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::BeginRead()
{
    n_assert(!readable);
    n_assert(!writable);
    SetPos(0);
    readable = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::EndRead()
{
    n_assert(readable);
    n_assert(!writable);
    readable = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBitStream::IsWritable() const
{
    return writable;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBitStream::IsReadable() const
{
    return readable;
}

//------------------------------------------------------------------------------
/**
    Print debug data to stdout.
*/
inline
void
nBitStream::Print(int lines)
{
    // require
    n_assert(stream != 0);

    // print out the first bits
    for (int i = 0; i < lines; i++)
    {
        n_printf("%4.d: ", i + 1);
        char ch = stream[i];
        for (int j = 0; j < 8; j++)
        {
            if (ch & (1 << (7 - j)))
            {
                n_printf("%d", 1);
            }
            else
            {
                n_printf("%d", 0);
            }
        }
        n_printf("\n");
    }
}

//------------------------------------------------------------------------------
/**
    Assignment operator.
*/
inline
nBitStream&
nBitStream::operator = (const nBitStream& s)
{
    n_assert(s.streamSize > 0);
    n_assert(s.streamSize == streamSize);
    n_assert(stream != 0);
    n_assert(s.stream != 0);

    Copy(s);

    return *this;
}

//------------------------------------------------------------------------------
/**
    Equality operator.
*/
inline
bool
nBitStream::operator == (const nBitStream& s) const
{
    if (streamSize != s.streamSize) return false;

    return (memcmp(stream, s.stream, streamSize) == 0);
}

//------------------------------------------------------------------------------
/**
    Inequality operator.
*/
inline
bool
nBitStream::operator != (const nBitStream& s) const
{
    if (streamSize != s.streamSize) return true;

    return (memcmp(stream, s.stream, streamSize) != 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBitStream::WriteBit(bool bit)
{
    n_assert(stream != 0);
    n_assert(BitsLeft() >= 1);
    n_assert(writable);

    // Set bit at current bit in current byte;
    // assume that all upcomming bits are cleared
    if (bit)
    {
        int v = (bit) ? 1 : 0;
        stream[currentByte] |= (v << (7 - currentBit));
    }

    // Advance to next byte if current bit is 8!
    if (++currentBit == 8)
    {
        currentByte++;
        currentBit = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBitStream::ReadBit()
{
    n_assert(stream != 0);
    n_assert(BitsLeft() >= 1);
    n_assert(IsReadable());

    // Read current bit
    bool flag = ((stream[currentByte] & (1 << (7 - currentBit))) != 0);

    // Advance to next byte if current bit is 8!
    if (++currentBit == 8)
    {
        currentByte++;
        currentBit = 0;
    }

    return flag;
}

#endif
