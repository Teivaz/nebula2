//------------------------------------------------------------------------------
//  ntqt2filemaker.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/ntqt2filemaker.h"

//------------------------------------------------------------------------------
/**
*/
nTqt2FileMaker::nTqt2FileMaker(nKernelServer* ks) :
    kernelServer(ks),
    srcFile(ks),
    tileSize(0),
    treeDepth(0),
    tarFile(0)
{
    // initialize IL and ILU
    ilInit();
    iluInit();

    // set ILU's scale filter
    iluImageParameter(ILU_FILTER, ILU_BILINEAR);
}

//------------------------------------------------------------------------------
/**
*/
nTqt2FileMaker::~nTqt2FileMaker()
{
    this->CloseFiles();
}

//------------------------------------------------------------------------------
/**
    Close all files.
*/
void
nTqt2FileMaker::CloseFiles()
{
    if (this->tarFile)
    {
        this->tarFile->Release();
        this->tarFile = 0;
    }
    if (this->srcFile.IsOpen())
    {
        this->srcFile.Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open all files.
*/
bool
nTqt2FileMaker::OpenFiles()
{
    n_assert(this->kernelServer);
    n_assert(!this->srcFileName.IsEmpty());
    n_assert(!this->tarFileName.IsEmpty());
    n_assert(0 == this->tarFile);

    nFileServer2* fileServer = kernelServer->GetFileServer();
    if (!this->srcFile.OpenRead(this->srcFileName.Get()))
    {
        switch (this->srcFile.GetError())
        {
            case nTgaFile::InvalidPixelFormat:
                this->SetError("Source file '%s' must be a 32 bit tga file!\n", this->GetSourceFile());
                break;

            default:
                this->SetError("Could not open source file '%s'\n", this->GetSourceFile());
                break;
        }
        return false;
    }

    this->tarFile = fileServer->NewFileObject();
    if (!this->tarFile->Open(this->tarFileName.Get(), "rwb"))
    {
        this->SetError("Could not open target file '%s'\n", this->tarFileName.Get());
        this->CloseFiles();
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTqt2FileMaker::Run()
{
    this->SetError("NoError");

    // check parameters
    if ((this->treeDepth < 1) || (this->treeDepth > 12))
    {
        this->SetError("Invalid tree depth (must be between 1 and 12)\n");
        return false;
    }
    // check that filesize is power of 2
    int loggedTileSize = 1 << n_frnd(n_log2(float(this->tileSize)));
    if ((this->tileSize <= 0) || (this->tileSize != loggedTileSize))
    {
        this->SetError("Tile size must be 2^n (is: %d)", this->tileSize);
        return false;
    }

    // open the files...
    if (!this->OpenFiles())
    {
        return false;
    }

    // write tqt2 header
    this->tarFile->PutInt('TQT2');
    this->tarFile->PutInt(this->treeDepth);
    this->tarFile->PutInt(this->tileSize);
    this->tarFile->PutInt(0);   // format, 0 is "RAW"

    // write tqt2 table of contents, record start of table of contents
    this->toc.SetFixedSize(this->CountNodes(this->treeDepth));
    struct TocEntry emptyTocEntry = { 0, 0 };
    this->toc.Fill(0, this->toc.Size(), emptyTocEntry);
    int tocStart = this->tarFile->Tell();
    int i;
    for (i = 0; i < this->toc.Size(); i++)
    {
        this->tarFile->PutInt(this->toc[i].pos);
        this->tarFile->PutInt(this->toc[i].size);
    }

    // create a source and a target DevIL image
    ILuint srcImage = iluGenImage();
    ILuint dstImage = iluGenImage();

    // prepare source image
    int tileDim = 1 << (this->treeDepth - 1);
    ilBindImage(srcImage);
    int srcImageSize = n_fchop((1.0f / tileDim) * this->srcFile.GetHeight());
    int srcImageBpp  = 4;
    int srcByteSize = srcImageSize * srcImageSize * srcImageBpp;
    ILenum srcFormat = IL_RGBA;
    ilTexImage(srcImageSize, srcImageSize, 1, srcImageBpp, srcFormat, IL_UNSIGNED_BYTE, 0);

    // generate base level chunks...
    n_printf("-> Generating base level chunks...\n");
    int dstByteSize = this->tileSize * this->tileSize * srcImageBpp;
    int row;
    for (row = 0; row < tileDim; row++)
    {
        int y = row * srcImageSize;
        int col;
        for (col = 0; col < tileDim; col++)
        {
            int x = col * srcImageSize;

            // load source image chunk
            ilBindImage(srcImage);
            ILubyte* srcData = ilGetData();
            if (!this->srcFile.ReadChunk(x, y, srcImageSize, srcImageSize, (char*)srcData, srcByteSize))
            {
                this->SetError("Error while loading tga chunk!");
                return false;
            }
            
            // copy to dst image and scale to destination size
            ilBindImage(dstImage);
            ilCopyImage(srcImage);
            iluScale(this->tileSize, this->tileSize, 1);

            // update table of contents in tqt2 file
            int filePos = this->tarFile->Tell();
            int quadTreeIndex = this->GetNodeIndex(this->treeDepth - 1, col, row);
            this->toc[quadTreeIndex].pos = filePos;
            this->toc[quadTreeIndex].size = dstByteSize;
            
            ILubyte* dstData = ilGetData();
            this->tarFile->Write(dstData, dstByteSize);

            n_printf("-> tile (level=%d, col=%d, row=%d) written\r", this->treeDepth - 1, col, row);
            fflush(stdout);
        }
    }
    iluDeleteImage(srcImage);
    iluDeleteImage(dstImage);

    // recursively generate sublevel tiles...
    n_printf("-> Generate sublevel chunks...\n");
    ILuint rootTileImage = this->RecurseGenerateTiles(0, 0, 0);
    iluDeleteImage(rootTileImage);

    // write TOC back to file
    this->tarFile->Seek(tocStart, nFile::START);
    for (i = 0; i < this->toc.Size(); i++)
    {
        this->tarFile->PutInt(this->toc[i].pos);
        this->tarFile->PutInt(this->toc[i].size);
    }

    // close and return
    this->CloseFiles();
    n_printf("-> Done.\n");
    return true;
}

//------------------------------------------------------------------------------
/**
    Copy one image into anther.
*/
void
nTqt2FileMaker::CopyImage(ILuint srcImage, ILuint dstImage, int dstX, int dstY)
{
    ilBindImage(srcImage);
    int srcW = ilGetInteger(IL_IMAGE_WIDTH);
    int srcH = ilGetInteger(IL_IMAGE_HEIGHT);
    uint* srcData = (uint*) ilGetData();
    ilBindImage(dstImage);
    int dstW = ilGetInteger(IL_IMAGE_WIDTH);
    // int dstH = ilGetInteger(IL_IMAGE_HEIGHT);
    uint* dstData = (uint*) ilGetData();
    int y;
    for (y = 0; y < srcH; y++)
    {
        int x;
        for (x = 0; x < srcW; x++)
        {
            uint* fromPtr = srcData + y * srcW + x;
            uint* toPtr = dstData + (y + dstY) * dstW + (x + dstX);
            *toPtr = *fromPtr;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Recursively generates the sublevel files by reading 4 chunks, combining them
    into one image, scaling them to the tile size and writing the chunk 
    back to the target file.
*/
ILuint
nTqt2FileMaker::RecurseGenerateTiles(int level, int col, int row)
{
    const int bytesPerPixel = 4;

    int quadIndex = this->GetNodeIndex(level, col, row);
    int filePos = this->toc[quadIndex].pos;
    int dataSize = this->toc[quadIndex].size;
    if (filePos > 0)
    {
        // tile alread built, read it from the file into a new IL image
        ILuint newImage = iluGenImage();
        ilBindImage(newImage);
        ilTexImage(this->tileSize, this->tileSize, 1, bytesPerPixel, IL_RGBA, IL_UNSIGNED_BYTE, 0); 
        ILubyte* data = ilGetData();
        this->tarFile->Seek(filePos, nFile::START);
        this->tarFile->Read(data, dataSize);
        return newImage;
    }

    // should never reach the bottom of the tree
    n_assert(level < (this->treeDepth - 1));

    // create an image which can hold 2x2 images
    ILuint tileImage = iluGenImage();
    ilBindImage(tileImage);
    ilTexImage(this->tileSize * 2, this->tileSize * 2, 1, bytesPerPixel, IL_RGBA, IL_UNSIGNED_BYTE, 0);
    ilClearColor(255, 0, 255, 255);
    ilClearImage();

    // resample the 4 children to make this tile
    int j;
    for (j = 0; j < 2; j++)
    {
        int i;
        for (i = 0; i < 2; i++)
        {
            int childCol = col * 2 + i;
            int childRow = row * 2 + j;
            ILuint childImage = this->RecurseGenerateTiles(level + 1, childCol, childRow);

            // copy the child image to its place in the tile image (topleft, topright, botleft or botright)
            int xOffset = i ? this->tileSize : 0;
            int yOffset = j ? this->tileSize : 0;
            //int yOffset = j ? 0 : this->tileSize;
            this->CopyImage(childImage, tileImage, xOffset, yOffset);

            // dispose of child image
            ilBindImage(childImage);
            iluDeleteImage(childImage);
        }
    }
    ilBindImage(tileImage);

    // scale tile image down to tile size
    iluScale(this->tileSize, this->tileSize, 1);

    // write the generated image to the tqt2 file
    this->tarFile->Seek(0, nFile::END);
    filePos = this->tarFile->Tell();
    ILubyte* dstData = ilGetData();
    const int byteSize = this->tileSize * this->tileSize * bytesPerPixel;
    this->toc[quadIndex].pos = filePos;
    this->toc[quadIndex].size = byteSize;
    this->tarFile->Write(dstData, byteSize);
    n_printf("  -> tile (level=%d, col=%d, row=%d) written\r", level, col, row);
    fflush(stdout);

    // return the tile image to the caller
    return tileImage;
}
