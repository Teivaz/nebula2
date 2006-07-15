#ifndef N_TEXTUREQUADTREE_H
#define N_TEXTUREQUADTREE_H
//------------------------------------------------------------------------------
/**
    @class nTextureQuadTree
    @ingroup NCTerrain2

    @brief Provides read-access to a chunked texture tqt2 file. Takes a tree
    level, and a chunk x/y coordinate in the level, and constructs a texture
    from it's contents.
    
    FIXME: find a texture object caching solution if creating/destroying
    texture objects on the fly is too slow (or move the problem out of
    nTextureQuadTree by providing an external texture???).
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nTextureQuadTree
{
public:
    /// file formats
    enum Format
    {
        RAW = 0,
        DDS = 1,
    };

    /// constructor
    nTextureQuadTree(nKernelServer* kernelServer, nGfxServer2* gfxServer);
    /// destructor
    ~nTextureQuadTree();
    /// open the tqt2 file
    bool Open(const char* filename);
    /// close the tqt2 file
    void Close();
    /// currently open?
    bool IsOpen() const;
    /// get the tree depth of the tqt2 file
    int GetTreeDepth() const;
    /// get the tile size of the tqt2 file
    int GetTileSize() const;
    /// get number of nodes including all child nodes in a level
    int GetNumNodes(int level) const;
    /// get node index by chunk address
    int GetNodeIndex(int level, int col, int row) const;
    /// construct a texture and fill with texture chunk data
    nTexture2* LoadTexture(int level, int col, int row);

private:
    /// a TOC entry structure
    struct TocEntry
    {
        int pos;
        int size;
    };

    nKernelServer* kernelServer;
    nGfxServer2* gfxServer;
    nFile* file;
    int fileSize;
    int treeDepth;
    int tileSize;
    int format;
    nArray<TocEntry> toc;
};

//------------------------------------------------------------------------------
/**
*/
inline
nTextureQuadTree::nTextureQuadTree(nKernelServer* ks, nGfxServer2* gfx) :
    kernelServer(ks),
    gfxServer(gfx),
    file(0),
    fileSize(0),
    treeDepth(0),
    tileSize(0),
    format(RAW),
    toc(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nTextureQuadTree::~nTextureQuadTree()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(0 == this->file);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTextureQuadTree::IsOpen() const
{
    return (0 != this->file);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTextureQuadTree::Open(const char* filename)
{
    n_assert(!this->IsOpen());
    n_assert(filename);
    
    // open tqt2 file
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if (!this->file->Open(filename, "rb"))
    {
        n_printf("nTextureQuadTree: could not load file '%s'\n", filename);
        this->Close();
        return false;
    }
    this->fileSize = this->file->GetSize();

    // read and verify header
    int magic = this->file->GetInt();
    if (magic != 'TQT2')
    {
        n_printf("nTextureQuadTree: '%s' not a tqt2 file!\n", filename);
        this->Close();
        return false;
    }

    // read header data
    this->treeDepth = this->file->GetInt();
    this->tileSize  = this->file->GetInt();
    this->format    = this->file->GetInt();

    // read table of contents
    int numNodes = this->GetNumNodes(this->treeDepth);
    this->toc.SetFixedSize(numNodes);
    int i;
    for (i = 0; i < numNodes; i++)
    {
        this->toc[i].pos = this->file->GetInt();
        this->toc[i].size = this->file->GetInt();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTextureQuadTree::Close()
{
    n_assert(this->IsOpen());
    this->file->Close();
    this->file->Release();
    this->file = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTextureQuadTree::GetTreeDepth() const
{
    n_assert(this->IsOpen());
    return this->treeDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nTextureQuadTree::GetTileSize() const
{
    n_assert(this->IsOpen());
    return this->tileSize;
}

//------------------------------------------------------------------------------
/**
    Computes number of nodes in a level, including its child nodes.
*/
inline
int
nTextureQuadTree::GetNumNodes(int level) const
{
    n_assert(this->IsOpen());
    return 0x55555555 & ((1 << level * 2) - 1);
}

//------------------------------------------------------------------------------
/**
    Computes a linear chunk index for a chunk address consisting of 
    level, col and row.
*/
inline
int
nTextureQuadTree::GetNodeIndex(int level, int col, int row) const
{
    n_assert((col >= 0) && (col < (1 << level)));
    n_assert((row >= 0) && (row < (1 << level)));
    return this->GetNumNodes(level) + (row << level) + col;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2*
nTextureQuadTree::LoadTexture(int level, int col, int row)
{
    n_assert(this->IsOpen());
    n_assert(this->gfxServer);

    // create a new texture object
    int nodeIndex = this->GetNodeIndex(level, col, row);
    int filePos = this->toc[nodeIndex].pos;
    int dataSize = this->toc[nodeIndex].size;

    nTexture2* tex = this->gfxServer->NewTexture(0);
    if (RAW == this->format)
    {
        // load raw ARGB image data
        tex->SetUsage(nTexture2::CreateFromRawCompoundFile);
        tex->SetType(nTexture2::TEXTURE_2D);
        tex->SetFormat(nTexture2::A8R8G8B8);
        tex->SetWidth(this->tileSize);
        tex->SetHeight(this->tileSize);
        tex->SetCompoundFileData(this->file, filePos, dataSize);
        tex->SetAsyncEnabled(true);
    }
    else
    {
        // load as dds file from inside compund file
        tex->SetUsage(nTexture2::CreateFromDDSCompoundFile);
        tex->SetCompoundFileData(this->file, filePos, dataSize);
        tex->SetAsyncEnabled(true);
    }
    bool texCreated = tex->Load();
    n_assert(texCreated);
    return tex;
}

//------------------------------------------------------------------------------
#endif

