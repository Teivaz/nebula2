#define N_IMPLEMENTS nNtxFile
//------------------------------------------------------------------------------
//  nntxfile.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nntxfile.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/**
*/
nNtxFile::nNtxFile(nKernelServer* ks) :
    kernelServer(ks),
    refFileServer("/sys/servers/file2"),
    openRead(false),
    openWrite(false),
    fileObject(0),
    numBlocks(0),
    curBlock(0),
    blocks(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNtxFile::~nNtxFile()
{
    n_assert(!this->openRead);
    n_assert(!this->openWrite);
    n_assert(0 == this->fileObject);
    n_assert(0 == this->blocks);
}

//------------------------------------------------------------------------------
/**
    Set number of blocks, this will initialize the blocks array. Setting the
    number of blocks makes only sense when writing an ntx file.
*/
void
nNtxFile::SetNumBlocks(int num)
{
    n_assert(0 == this->blocks);
    this->numBlocks = num;
    this->blocks = new nNtxBlock[num];
}

//------------------------------------------------------------------------------
/**
    Get number of blocks. This is normally used after opening a file
    for reading to query the number of image data blocks in the file.
*/
int
nNtxFile::GetNumBlocks()
{
    return this->numBlocks;
}

//------------------------------------------------------------------------------
/**
    Sets current block which will be used by the methods which set or
    get block attributes.
*/
void
nNtxFile::SetCurrentBlock(int current)
{
    n_assert((current >= 0) && (current < this->numBlocks));
    this->curBlock = current;
}

//------------------------------------------------------------------------------
/**
    Get index of current block.
*/
int
nNtxFile::GetCurrentBlock()
{
    return this->curBlock;
}

//------------------------------------------------------------------------------
/**
    Find block index which matches the requested type, format and mipmap level.

    @return     index of matching block or 0 if no block found
*/
int
nNtxFile::FindBlock(nNtxFile::Type type, nNtxFile::Format format, int mipLevel)
{
    n_assert(this->blocks);

    int i;
    for (i = 0; i < this->numBlocks; i++)
    {
        if ((this->blocks[i].type == type) && 
            (this->blocks[i].format == format) && 
            (this->blocks[i].mipLevel == mipLevel))
        {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    Free all data blocks.
*/
void
nNtxFile::FreeBlocks()
{
    n_assert(this->blocks);
    
    delete[] this->blocks;
    
    this->blocks    = 0;
    this->numBlocks = 0;
    this->curBlock  = 0;
}

//------------------------------------------------------------------------------
/**
    Set the width of the current block, this is not allowed in read mode.
*/
void
nNtxFile::SetWidth(int width)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    this->blocks[this->curBlock].width = width;
}

//------------------------------------------------------------------------------
/**
    Get the width of the current block, this is only allowed in read or
    write mode.
*/
int
nNtxFile::GetWidth()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].width;
}

//------------------------------------------------------------------------------
/**
    Set the height of the current block, this is not allowed in read mode.
*/
void
nNtxFile::SetHeight(int height)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    this->blocks[this->curBlock].height = height;
}

//------------------------------------------------------------------------------
/**
    Get the height of the current block, this is only allowed in read or
    write mode.
*/
int
nNtxFile::GetHeight()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].height;
}

//------------------------------------------------------------------------------
/**
    Set the depth of the current block, this is not allowed in read mode.
*/
void
nNtxFile::SetDepth(int depth)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    this->blocks[this->curBlock].depth = depth;
}

//------------------------------------------------------------------------------
/**
    Get the depth of the current block, this is only allowed in read or
    write mode.
*/
int
nNtxFile::GetDepth()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].depth;
}

//------------------------------------------------------------------------------
/**
    Get the mip level of the current block, this is not allowed in read mode.
*/
void
nNtxFile::SetMipLevel(int mipLevel)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    this->blocks[this->curBlock].mipLevel = mipLevel;
}

//------------------------------------------------------------------------------
/**
    Get the mip level of the current block, this is only allowed in read or
    write mode.
*/
int
nNtxFile::GetMipLevel()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].mipLevel;
}

//------------------------------------------------------------------------------
/**
    Set the block type.
*/
void
nNtxFile::SetType(nNtxFile::Type type)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    this->blocks[this->curBlock].type = type;
}

//------------------------------------------------------------------------------
/**
    Get the block type.
*/
nNtxFile::Type
nNtxFile::GetType()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].type;
}

//------------------------------------------------------------------------------
/**
    Set the block pixel format.
*/
void
nNtxFile::SetFormat(nNtxFile::Format format)
{
    n_assert(this->blocks);
    n_assert(!this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    this->blocks[this->curBlock].format = format;
}

//------------------------------------------------------------------------------
/**
    Get the block pixel format.
*/
nNtxFile::Format
nNtxFile::GetFormat()
{
    n_assert(this->blocks);
    n_assert(this->openRead || this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].format;
}

//------------------------------------------------------------------------------
/**
    Get size of image data of current block in bytes.
*/
int
nNtxFile::GetSize()
{
    n_assert(this->blocks);
    n_assert(this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    return this->blocks[this->curBlock].dataSize;
}

//------------------------------------------------------------------------------
/**
    Get bytes per row of image data of current block in bytes.
*/
int
nNtxFile::GetBytesPerRow()
{
    n_assert(this->blocks);
    n_assert(this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));
    
    if (this->GetType() == TYPE_TEXTURECUBE)
    {
        return this->blocks[this->curBlock].dataSize / (this->blocks[this->curBlock].height * 6);
    }
    else
    {
        return this->blocks[this->curBlock].dataSize / this->blocks[this->curBlock].height;
    }
}

//------------------------------------------------------------------------------
/**
    Get bytes per pixel of image data of current block.
*/
int 
nNtxFile::GetBytesPerPixel()
{
    n_assert(this->blocks);
    n_assert(this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    nNtxBlock* block = &(this->blocks[this->curBlock]);
    if (this->GetType() == TYPE_TEXTURECUBE)
    {
        return block->dataSize / (block->width * block->height * 6);
    }
    else
    {
        return block->dataSize / (block->width * block->height);
    }
}

//------------------------------------------------------------------------------
/**
    Reads a 32 bit int from file. FIXME: Endian correction!
*/
int
nNtxFile::readInt()
{
    n_assert(this->fileObject);
    int var;
    this->fileObject->Read(&var, sizeof(var));
    return var;
}

//------------------------------------------------------------------------------
/**
    Writes a 32 bit int to file. FIXME: Endian correction!
*/
void
nNtxFile::writeInt(const int var)
{
    n_assert(this->openWrite);
    n_assert(this->fileObject);
    this->fileObject->Write(&var, sizeof(var));
}

//------------------------------------------------------------------------------
/**
    Fill current block header with data from current file position.
*/
void
nNtxFile::readBlockHeader()
{
    n_assert(this->blocks);
    n_assert(this->openRead);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    nNtxBlock* block  = &(this->blocks[this->curBlock]);
    block->format     = (nNtxFile::Format) this->readInt();
    block->type       = (nNtxFile::Type)   this->readInt();
    block->width      = this->readInt();
    block->height     = this->readInt();
    block->depth      = this->readInt();
    block->mipLevel   = this->readInt();
    block->dataOffset = this->readInt();
    block->dataSize   = this->readInt();
}

//------------------------------------------------------------------------------
/**
    Write current block header to current file position.
*/
void
nNtxFile::writeBlockHeader()
{
    n_assert(this->blocks);
    n_assert(this->openWrite);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    nNtxBlock* block = &(this->blocks[this->curBlock]);
    this->writeInt(block->format);
    this->writeInt(block->type);
    this->writeInt(block->width);
    this->writeInt(block->height);
    this->writeInt(block->depth);
    this->writeInt(block->mipLevel);
    this->writeInt(block->dataOffset);
    this->writeInt(block->dataSize);
}

//------------------------------------------------------------------------------
/**
    Reads file header, verifies that it is a NTX1 file, and allocates the
    required number of blocks, reads the block headers and sets block 0
    as the current block.

    @return     false if not a NTX1 file
*/
bool
nNtxFile::readFileHeader()
{
    n_assert(!this->blocks);
    n_assert(this->openRead);

    // read magic number
    int magic = this->readInt();
    if ('NTX1' == magic)
    {
        // read number of blocks in file
        this->SetNumBlocks(this->readInt());

        // read block headers
        int i;
        for (i = 0; i < this->numBlocks; i++)
        {
            this->SetCurrentBlock(i);
            this->readBlockHeader();
        }
        this->SetCurrentBlock(0);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Writes the file header and all block headers, computes data offsets and
    data sizes on the fly. Leaves file position at end of block headers on
    return. Sets 0 as the current block.
*/
void
nNtxFile::writeFileHeader()
{
    n_assert(this->blocks);
    n_assert(this->numBlocks > 0);
    n_assert(this->openWrite);

    // write magic number
    this->writeInt('NTX1');

    // write number of blocks
    this->writeInt(this->numBlocks);

    // update the data block offsets and sizes
    this->updateDataBlockOffsets();

    // write block headers
    int i;
    for (i = 0; i < this->numBlocks; i++)
    {
        this->SetCurrentBlock(i);
        this->writeBlockHeader();
    }
    this->SetCurrentBlock(0);
}

//------------------------------------------------------------------------------
/**
    For each block header, compute offset and size of its data block in the
    file and update the respective block header members in memory.
*/
void
nNtxFile::updateDataBlockOffsets()
{
    n_assert(this->blocks);
    n_assert(this->numBlocks > 0);
    
    // start of first data block
    int blockOffset = 
        sizeof(int) +       // 'NTX1'
        sizeof(int) +       // numBlocks
        this->numBlocks * nNtxBlock::NUMMEMBERS * sizeof(int);  // the blocks

    // for each block...
    int i;
    for (i = 0; i < this->numBlocks; i++)
    {
        nNtxBlock* block = &(this->blocks[i]);
        int pixelSize;
        switch(block->format)
        {
            case FORMAT_R8G8B8:
                pixelSize = 3;
                break;
            case FORMAT_A8R8G8B8:
                pixelSize = 4;
                break;
            case FORMAT_R5G6B5:
            case FORMAT_A4R4G4B4:
                pixelSize = 2;
                break;
            default:
                n_error("nNtxFile::updateDataBlockOffset(): illegal block format\n");
                break;
        }
        int blockSize = block->width * block->height * block->depth * pixelSize;
        if (block->type == TYPE_TEXTURECUBE)
        {
            blockSize *= 6;
        }

        this->blocks[i].dataOffset = blockOffset;
        this->blocks[i].dataSize   = blockSize;

        blockOffset += blockSize;
    }
}

//------------------------------------------------------------------------------
/**
    Open file for writing. Writing a nNtxFile object should be done like
    this:

       -# configure block headers (SetNumBlocks(), SetCurrentBlock(), SetWidth(), ...)
       -# OpenWrite()
       -# for each block WriteBlock()
       -# CloseWrite()
*/
bool
nNtxFile::OpenWrite(const char* fileName)
{
    n_assert(fileName);
    n_assert(0 == this->fileObject);
    n_assert(!(this->openWrite || this->openRead));
    n_assert(this->blocks);
    n_assert(this->numBlocks > 0);

    this->fileObject = this->refFileServer->NewFileObject();
    n_assert(fileObject);

    if (this->fileObject->Open(fileName, "w"))
    {
        this->openWrite = true;
        this->writeFileHeader();
        return true;
    }
    else
    {
        n_printf("nNtxFile::OpenWrite(): could not open file '%s' for writing!\n", fileName);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Close file after writing, leaves block headers untouched.
*/
void
nNtxFile::CloseWrite()
{
    n_assert(this->openWrite && (!this->openRead));
    n_assert(this->fileObject);

    this->fileObject->Close();
    delete this->fileObject;
    this->fileObject = 0;
    this->openWrite = false;
}

//------------------------------------------------------------------------------
/**
    Write memory buffer of current block to file. The block data must
    be written completely at once!
*/
int
nNtxFile::WriteBlock(void* buf, int bufSize)
{
    n_assert(this->openWrite && (!this->openRead));
    n_assert(this->fileObject);
    n_assert(this->blocks);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    nNtxBlock* block = &(this->blocks[this->curBlock]);
    n_assert(bufSize == block->dataSize);

    // seek to file pos of current block
    this->fileObject->Seek(block->dataOffset, nFile::START);

    // write data
    return this->fileObject->Write(buf, bufSize);
}

//------------------------------------------------------------------------------
/**
    Open NTX file for reading. This will read the file header, make
    sure that it is a valid NTX1 file, and then read all block headers.
    After OpenRead() returns successfully you can query the number of
    blocks, block types, dimensions, formats etc, read the block data
    with ReadBlockData() and finally close with CloseRead(). The block
    data will still be valid after CloseRead(), either free them with
    FreeBlocks(), or use for a write operation.
*/
bool
nNtxFile::OpenRead(const char* fileName)
{
    n_assert(fileName);
    n_assert(0 == this->fileObject);
    n_assert(!(this->openRead || this->openWrite));
    n_assert(!this->blocks);
    n_assert(this->numBlocks == 0);

    this->fileObject = this->refFileServer->NewFileObject();
    n_assert(this->fileObject);
    
    if (this->fileObject->Open(fileName, "rb"))
    {
        this->openRead = true;
        this->readFileHeader();
        return true;
    }
    else
    {
        n_printf("nNtxFile::OpenRead(): could not open file '%s' for reading!\n", fileName);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Close the file after reading. The data block headers of the read operation
    will remain intact until freed with FreeBlocks().
*/
void
nNtxFile::CloseRead()
{
    n_assert(this->openRead && (!this->openWrite));
    n_assert(this->fileObject);

    this->fileObject->Close();
    delete this->fileObject;
    this->fileObject = 0;
    this->openRead = false;
}

//------------------------------------------------------------------------------
/**
    Read block data of current block into memory buffer. The block data
    must be read completely at once.
*/
int
nNtxFile::ReadBlock(void* buf, int bufSize)
{
    n_assert(this->openRead && (!this->openWrite));
    n_assert(this->fileObject);
    n_assert(this->blocks);
    n_assert((this->curBlock >= 0) && (this->curBlock < this->numBlocks));

    nNtxBlock* block = &(this->blocks[this->curBlock]);
    n_assert(bufSize >= block->dataSize);

    // seek to file pos of current block
    this->fileObject->Seek(block->dataOffset, nFile::START);

    // write data
    return this->fileObject->Read(buf, block->dataSize);
}

//------------------------------------------------------------------------------














