#ifndef N_TGAFILE_H
#define N_TGAFILE_H
//------------------------------------------------------------------------------
/**
    @class nTgaFile
    @ingroup NCTerrain2Tools

    @brief A simple tga file reader which can read subsegments of very large 
    files.

    DevIL cannot read subchunks, that's why this class exists. Can only read
    uncompressed 32 bit TGA files.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nTgaFile
{
public:
    /// error codes
    enum Error
    {
        Success,
        CouldNotOpenFile,
        InvalidPixelFormat,
        InvalidChunkSize,
        BufferTooSmall
    };

    /// constructor
    nTgaFile(nKernelServer* kernelServer);
    /// destructor
    ~nTgaFile();
    /// open the file in read mode
    bool OpenRead(const char* filename);
    /// open the file in write mode
    bool OpenWrite(const char* filename);
    /// close the file
    void Close();
    /// currently open?
    bool IsOpen() const;
    /// set width of file
    void SetWidth(int w);
    /// get width of file
    int GetWidth() const;
    /// set height of file
    void SetHeight(int h);
    /// get height of file
    int GetHeight() const;
    /// get bytes per pixel
    int GetBytesPerPixel() const;
    /// get bytes per row
    int GetBytesPerRow() const;
    /// get error code, if Open() returns false
    Error GetError() const;
    /// read a subchunk of the image into the provided buffer
    bool ReadChunk(int x, int y, int w, int h, char* buffer, int bufSize);
    /// write a chunk of data into the file
    bool WriteChunk(int x, int y, int w, int h, char* buffer);

private:
    nKernelServer* kernelServer;
    nFile* file;
    int width;
    int height;
    int dataOffset;         // offset of image data from start of file
    Error error;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTgaFile::IsOpen() const
{
    return (0 != this->file);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTgaFile::SetWidth(int w)
{
    n_assert(!this->IsOpen());
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTgaFile::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTgaFile::SetHeight(int h)
{
    n_assert(!this->IsOpen());
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTgaFile::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTgaFile::GetBytesPerPixel() const
{
    return 4;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTgaFile::GetBytesPerRow() const
{
    return this->width * this->GetBytesPerPixel();
}

//------------------------------------------------------------------------------
/**
*/
inline
nTgaFile::Error
nTgaFile::GetError() const
{
    return this->error;
}

//------------------------------------------------------------------------------
#endif

