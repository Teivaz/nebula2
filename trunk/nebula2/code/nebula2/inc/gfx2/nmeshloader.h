#ifndef N_MESHLOADER_H
#define N_MESHLOADER_H
//------------------------------------------------------------------------------
/**
    @class nMeshLoader

    Base class for specific mesh loaders. Mesh loaders load mesh data
    from a specific file format into user-provided memory blocks
    for vertex and index data.
    
    nMeshLoader is a non-functional base class. Use the specialized
    classes nN3d2Loader and nNvx2Loader to load mesh files of those
    formats.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/narray.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nmeshgroup.h"
#include "util/npathstring.h"

//------------------------------------------------------------------------------
class nMeshLoader
{
public:
    /// index types
    enum IndexType
    {
        Index16,
        Index32,
    };

    /// constructor
    nMeshLoader();
    /// destructor
    ~nMeshLoader();
    /// set filename
    void SetFilename(const char* name);
    /// get filename
    const char* GetFilename() const;
    /// 16 or 32 bit indices (default is 16)
    void SetIndexType(IndexType t);
    /// get index type
    IndexType GetIndexType() const;
    /// open the file and read header data
    bool Open(nFileServer2* fileServer);
    /// close the file
    void Close();
    /// get number of groups (valid after Open())
    int GetNumGroups() const;
    /// get group info (valid after Open())
    const nMeshGroup& GetGroupAt(int index) const;
    /// get number of vertices (valid after Open())
    int GetNumVertices() const;
    /// get vertex width (aka stride) in sizeof(float)'s (valid after Open())
    int GetVertexWidth() const;
    /// get number of triangles (valid after Open())
    int GetNumTriangles() const;
    /// get number of indices (valid after Open())
    int GetNumIndices() const;
    /// get vertex components (see gfx2/nmesh2.h)(valid after Open())
    int GetVertexComponents() const;
    /// read vertex data
    bool ReadVertices(void* buffer, int bufferSize);
    /// read index data
    bool ReadIndices(void* buffer, int bufferSize);

protected:
    nFileServer2* fileServer;
    nPathString filename;
    nFile* file;
    IndexType indexType;
    int numGroups;
    int numVertices;
    int vertexWidth;
    int numTriangles;
    int numIndices;
    int vertexComponents;
    nArray<nMeshGroup> groupArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMeshLoader::nMeshLoader() :
    fileServer(0),
    file(0),
    indexType(Index16),
    numGroups(0),
    numVertices(0),
    vertexWidth(0),
    numTriangles(0),
    numIndices(0),
    vertexComponents(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshLoader::~nMeshLoader()
{
    n_assert(0 == this->file);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshLoader::SetFilename(const char* name)
{
    n_assert(name);
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nMeshLoader::GetFilename() const
{
    return this->filename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshLoader::SetIndexType(IndexType t)
{
    this->indexType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshLoader::IndexType
nMeshLoader::GetIndexType() const
{
    return this->indexType;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshLoader::Open(nFileServer2* fileServer)
{
    n_error("nMeshLoader::Open() called!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshLoader::Close()
{
    n_error("nMeshLoader::Close() called!");
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetNumGroups() const
{
    return this->numGroups;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nMeshGroup& 
nMeshLoader::GetGroupAt(int index) const
{
    return this->groupArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetVertexWidth() const
{
    return this->vertexWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetNumTriangles() const
{
    return this->numTriangles;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshLoader::GetVertexComponents() const
{
    return this->vertexComponents;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshLoader::ReadVertices(void* buffer, int bufferSize)
{
    n_error("nMeshLoader::ReadVertices() called!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshLoader::ReadIndices(void* buffer, int bufferSize)
{
    n_error("nMeshLoader::ReadIndices() called!");
    return false;
}

//------------------------------------------------------------------------------
#endif

