#ifndef N_FILE_H
#define N_FILE_H
//------------------------------------------------------------------------------
/**
    @class nFile
    @ingroup NebulaFileManagement

    wrapper for file routines

    provides functions for reading and writing files

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nfiletime.h"
#include "kernel/nrefcounted.h"

#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#elif defined(__WIN32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#else
#include <stdio.h>
#endif

class nFileServer2;

//------------------------------------------------------------------------------
class nFile : public nRefCounted
{
public:
    /// start point for seeking in file
    enum nSeekType
    {
        CURRENT,
        START,
        END,
    };

    /// constructor
    nFile(nFileServer2* server);
    /// opens a file
    virtual bool Open(const char* fileName, const char* accessMode);
    /// closes the file
    virtual void Close();
    /// writes some bytes to the file
    virtual int Write(const void* buffer, int numBytes);
    /// reads some bytes from the file
    virtual int Read(void* buffer, int numBytes);
    /// gets actual position in file
    virtual int Tell();
    /// sets new position in file
    virtual bool Seek(int byteOffset, nSeekType origin);
    /// is the file at the end
    virtual bool Eof();
    /// get size of file in bytes
    virtual int GetSize() const;
    /// get the last write time
    virtual nFileTime GetLastWriteTime() const;
    /// writes a string to the file
    virtual bool PutS(const char* buffer);
    /// reads a string from the file
    virtual bool GetS(char* buffer, int numChars);
    /// get current line number (incremented by PutS() and GetS())
    int GetLineNumber() const;
    /// determines wether the file is opened
    bool IsOpen() const;
    /// append one file to another file
    virtual int AppendFile(nFile* other);
    /// write a 32bit int to the file
    int PutInt(int val);
    /// write a 16bit int to the file
    int PutShort(short val);
    /// write a 8bit int to the file
    int PutChar(char val);
    /// write a float to the file
    int PutFloat(float val);
    /// write a double to the file
    int PutDouble(double val);
    /// read a 32 bit int from the file
    int GetInt();
    /// read a signed 16 bit int from the file
    short GetShort();
    /// read an unsigned 16 bit int from the file
    ushort GetUShort();
    /// read a 8 bit int from the file
    char GetChar();
    /// read a float from the file
    float GetFloat();
    /// read a double from the file
    double GetDouble();

protected:
    /// destructor
    virtual ~nFile();

    nFileServer2* fs;
    ushort lineNumber;
    bool isOpen;

    #ifdef __WIN32__
        // win32 file handle
        HANDLE handle;
    #else
        // ansi c file pointer
        FILE* fp;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFile::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    @return     the current line number (only valid with GetS()/PutS())
*/
inline
int
nFile::GetLineNumber() const
{
    return this->lineNumber;
}

//------------------------------------------------------------------------------
/**
    @param   val    a 32 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutInt(int val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 16 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutShort(short val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 8 bit int value
    @return         number of bytes written
*/
inline
int
nFile::PutChar(char val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 32 bit float value
    @return         number of bytes written
*/
inline
int
nFile::PutFloat(float val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @param  val     a 64 bit double value
    @return         number of bytes written
*/
inline
int
nFile::PutDouble(double val)
{
    return this->Write(&val, sizeof(val));
}

//------------------------------------------------------------------------------
/**
    @return     the value
*/
inline
int
nFile::GetInt()
{
    int val = 0;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
/**
    @return     the value
*/
inline
short
nFile::GetShort()
{
    short val;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
/**
    @return     the value
*/
inline
ushort
nFile::GetUShort()
{
    ushort val;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
/**
    @return     the value
*/
inline
char
nFile::GetChar()
{
    char val;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 32 bit float value
    @return         number of bytes read
*/
inline
float
nFile::GetFloat()
{
    float val;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
/**
    @param  val     [out] 32 bit float value
    @return         number of bytes read
*/
inline
double
nFile::GetDouble()
{
    double val;
    this->Read(&val, sizeof(val));
    return val;
}

//------------------------------------------------------------------------------
#endif
