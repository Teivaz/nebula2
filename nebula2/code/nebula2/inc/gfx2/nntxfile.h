#ifndef N_NTXFILE_H
#define N_NTXFILE_H
//------------------------------------------------------------------------------
/**
    @class nNtxFile
    @ingroup NebulaGraphicsSystem

    Implement a Nebula texture file format optimized for fast loading.

    NTX files (Nebula Texture File) implements a simple texture file format
    for fast loading, the idea is that the data can simply be read from
    disk in one go without decompression or multiple reads. The image
    data lives in the file in several preprocessed blocks (for instance
    several pixel formats in one file, mipmaps, etc...).

    Here's the file format description:
    
    @verbatim
    ntxFile
    {
        int32 'NTX1'        -> magic number for NTX file format version 1
        int32  numBlocks    -> number of blocks following

        [numBlocks x ntxBlockHeader]

        [numBlocks x nBlockData]
    }

    ntxBlockHeader
    {
        int32 format        -> pixel format of block (== nNtxFile::Format)
        int32 type          -> block type (== nNtxFile::Type)
        int32 width         -> pixel width of block
        int32 heigth        -> pixel height of block
        int32 depth         -> pixel depth of block (for 3d textures)
        int32 miplevel      -> mipmap level
        int32 dataOffset    -> byte index into file to start of block data
        int32 dataSize      -> byte size of block data
    }

    ntxBlockData
    {
        ... simply a block of bytes with the image data
    }
    @endverbatim

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nNtxFile
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nFileServer2;
class nFile;
class N_PUBLIC nNtxFile
{
public:
    /// ntx pixel formats (compatible with respective D3D pixel formats)
    enum Format
    {
        FORMAT_NONE      = 0,
        FORMAT_R8G8B8    = 1,
        FORMAT_A8R8G8B8  = 2,
        FORMAT_R5G6B5    = 3,
        FORMAT_A4R4G4B4  = 4,
    };

    /// ntx block type
    enum Type
    {
        TYPE_NONE        = 0,
        TYPE_TEXTURE1D   = 1, 
        TYPE_TEXTURE2D   = 2,
        TYPE_TEXTURE3D   = 3,
        TYPE_TEXTURECUBE = 4,
    };

    /// constructor
    nNtxFile(nKernelServer* ks);
    /// destructor
    ~nNtxFile();

    /// open file for reading
    bool OpenRead(const char* fileName);
    /// close file after reading
    void CloseRead();
    /// read current block into memory buffer
    int ReadBlock(void* buf, int bufSize);

    /// open file for writing
    bool OpenWrite(const char* fileName);
    /// close file after writing
    void CloseWrite();
    /// append-write memory buffer to block
    int WriteBlock(void* buf, int bufSize);

    /// set number of blocks in image file (write)
    void SetNumBlocks(int num);
    /// get number of blocks in image file (read/write)
    int GetNumBlocks();
    /// set current block
    void SetCurrentBlock(int index);
    /// get current block
    int GetCurrentBlock();
    /// find block index with matching format and mipmap level
    int FindBlock(Type type, Format format, int mipLevel);
    /// free block data
    void FreeBlocks();

    /// set width of current block (write)
    void SetWidth(int width);
    /// get width of current block (read/write)
    int GetWidth();
    /// set heigth of current block (write)
    void SetHeight(int height);
    /// get height of current block (read/write)
    int GetHeight();
    /// set depth of current block (write)
    void SetDepth(int depth);
    /// get depth of current block (read/write)
    int GetDepth();
    /// set the mipmap level of the current block (write)
    void SetMipLevel(int level);
    /// get the mipmap level of the current block (read/write)
    int GetMipLevel();
    /// set block type (write)
    void SetType(nNtxFile::Type type);
    /// get block type (read/write)
    nNtxFile::Type GetType();
    /// set pixel format of current block (write)
    void SetFormat(nNtxFile::Format format);
    /// get pixel format of current block (read/write)
    nNtxFile::Format GetFormat();
    /// get the size of the data block in bytes
    int GetSize();
    /// get bytes per row of data block
    int GetBytesPerRow();
    /// get bytes per pixel of data block
    int GetBytesPerPixel();

private:
    /// the default constructor is private
    nNtxFile();
    /// read a 32 bit int from file
    int readInt();
    /// write a 32 bit int to file
    void writeInt(const int var);
    /// read current block header
    void readBlockHeader();
    /// write current block header to file
    void writeBlockHeader();
    /// read file header from file
    bool readFileHeader();
    /// write file header to file
    void writeFileHeader();
    /// update data block offsets and sizes
    void updateDataBlockOffsets();

    class nNtxBlock
    {
        friend class nNtxFile;

        /// constructor
        nNtxBlock() :
            format(FORMAT_NONE),
            type(TYPE_NONE),
            width(0),
            height(0),
            depth(0),
            mipLevel(0),
            dataOffset(0),
            dataSize(0)
        {
            // empty
        }        
        Format format;
        Type   type;
        int width;
        int height;
        int depth;
        int mipLevel;
        int dataOffset;
        int dataSize;
        enum
        {
            NUMMEMBERS = 8,
        };
    };

    nKernelServer* kernelServer;
    nAutoRef<nFileServer2> refFileServer;
    bool openRead;          // currently open for reading
    bool openWrite;         // currently open for writing
    nFile* fileObject;      // file handle
    int numBlocks;          // number of data blocks in file
    int curBlock;           // current block number
    nNtxBlock* blocks;      // pointer to block objects
};

//------------------------------------------------------------------------------
/**
    The default constructor is private and may not be used!
*/
inline
nNtxFile::nNtxFile() :
    refFileServer(0)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
