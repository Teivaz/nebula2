//------------------------------------------------------------------------------
//  ntgafile.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/ntgafile.h"

//------------------------------------------------------------------------------
/**
*/
nTgaFile::nTgaFile(nKernelServer* ks) :
    kernelServer(ks),
    file(0),
    width(0),
    height(0),
    dataOffset(0),
    error(Success)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTgaFile::~nTgaFile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the file in read mode.
*/
bool
nTgaFile::OpenRead(const char* filename)
{
    n_assert(this->kernelServer);
    n_assert(0 == this->file);

    // open file
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if (!this->file->Open(filename, "rb"))
    {
        this->error = CouldNotOpenFile;
        file->Release();
        return false;
    }

    // read header
    char idFieldSize = file->GetChar();
    file->GetChar();     // colorMapType
    file->GetChar();     // imageType
    file->GetShort();    // colorMapStart
    file->GetShort();    // colorMapLength
    file->GetChar();     // colorMapBits
    file->GetShort();    // xStart
    file->GetShort();    // yStart
    this->width  = file->GetShort();
    this->height = file->GetShort();
    char bitsPerPixel = file->GetChar();
    file->GetChar();    // descriptor

    // correct pixel format?
    if (bitsPerPixel != 32)
    {
        this->error = InvalidPixelFormat;
        file->Release();
        return false;
    }

    // fill remaining object attributes
    const int headerSize = 18;
    this->dataOffset = headerSize + idFieldSize;

    this->error = Success;
    return true;
}

//------------------------------------------------------------------------------
/**
    Open the chunk in write mode.
*/
bool
nTgaFile::OpenWrite(const char* filename)
{
    n_assert(this->GetWidth() > 0);
    n_assert(this->GetHeight() > 0);
    n_assert(filename);

    n_assert(this->kernelServer);
    n_assert(0 == this->file);

    // open file
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if (!this->file->Open(filename, "wb"))
    {
        this->error = CouldNotOpenFile;
        file->Release();
        return false;
    }

    // write tga header
    file->PutChar(0);       // identsize
    file->PutChar(0);       // colormap type = none
    file->PutChar(2);       // imagetype = rgb
    file->PutShort(0);      // colormapstart
    file->PutShort(0);      // colormaplength
    file->PutChar(0);       // colormapbits
    file->PutShort(0);      // x origin
    file->PutShort(0);      // y origin
    file->PutShort(this->width);    // width
    file->PutShort(this->height);   // height
    file->PutChar(32);              // bits per pixel
    file->PutChar(8);               // h/v flip, alpha bits

    // standard header of 18 bytes
    this->dataOffset = 18;

    // write data (all violet pixels)
    uint* row = n_new_array(uint, this->width);
    int i;
    for (i = 0; i < this->width; i++)
    {
        row[i] = 0xffff00ff;
    }
    for (i = 0; i < this->height; i++)
    {
        file->Write(row, this->width * sizeof(uint));
    }
    n_delete_array(row);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nTgaFile::Close()
{
    n_assert(this->file);
    this->file->Close();
    this->file->Release();
    this->file = 0;
}

//------------------------------------------------------------------------------
/**
    Read a chunk from the image defined by position and size in pixels into
    the provided buffer. Returns true on success, otherwise sets ErrorCodes
    InvalidChunkSize or BufferTooSmall.
*/
bool
nTgaFile::ReadChunk(int x, int y, int w, int h, char* buffer, int bufSize)
{
    n_assert(this->file);
    n_assert(buffer);

    // check for parameter errors
    if (bufSize < (w * h * this->GetBytesPerPixel()))
    {
        this->error = BufferTooSmall;
        return false;
    }
    if (((x + w) > this->width) || (y + h) > this->height)
    {
        this->error = InvalidChunkSize;
        return false;
    }

    int bytesPerPixel = this->GetBytesPerPixel();
    int bytesPerRow = this->GetBytesPerRow();
    int startPos = y * bytesPerRow + x * bytesPerPixel + this->dataOffset;

    // number of bytes to read per row
    int readBytesPerRow = w * bytesPerPixel;
    int row;
    char* ptr = buffer;
    for (row = 0; row < h; row++)
    {
        file->Seek(startPos + row * bytesPerRow, nFile::START);
        file->Read(ptr, readBytesPerRow);
        ptr += readBytesPerRow;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Write a chunk to the tga image.
*/
bool
nTgaFile::WriteChunk(int x, int y, int w, int h, char* buffer)
{
    n_assert(this->file);
    n_assert(buffer);

    // check for parameter errors
    if (((x + w) > this->width) || (y + h) > this->height)
    {
        this->error = InvalidChunkSize;
        return false;
    }

    int bytesPerPixel = this->GetBytesPerPixel();
    int bytesPerRow = this->GetBytesPerRow();
    int startPos = y * bytesPerRow + x * bytesPerPixel + this->dataOffset;

    // number of bytes to write per row
    int writeBytesPerRow = w * bytesPerPixel;
    int row;
    char* ptr = buffer;
    for (row = 0; row < h; row++)
    {
        file->Seek(startPos + row * bytesPerRow, nFile::START);
        file->Write(ptr, writeBytesPerRow);
        ptr += writeBytesPerRow;
    }
    return true;
}
