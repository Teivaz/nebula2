#ifndef N_NVX2LOADER_H
#define N_NVX2LOADER_H
//------------------------------------------------------------------------------
/**
    @class nNvx2Loader
    @ingroup Gfx2

    Load a NVX2 mesh file into user provided vertex and index buffers.

    (C) 2003 RadonLabs GmbH
*/
#include "gfx2/nmeshloader.h"

//------------------------------------------------------------------------------
class nNvx2Loader : public nMeshLoader
{
public:
    /// constructor
    nNvx2Loader();
    /// destructor
    virtual ~nNvx2Loader();
    /// open file and read header data
    virtual bool Open();
    /// close the file
    virtual void Close();
    /// read vertex data
    virtual bool ReadVertices(void* buffer, int bufferSize);
    /// read index data
    virtual bool ReadIndices(void* buffer, int bufferSize);
    /// read edge data
    virtual bool ReadEdges(void* buffer, int bufferSize);
};

//------------------------------------------------------------------------------
/**
*/
inline
nNvx2Loader::nNvx2Loader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nNvx2Loader::~nNvx2Loader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nNvx2Loader::Open()
{
    n_assert(!this->file);

    this->file = nFileServer2::Instance()->NewFileObject();
    n_assert(this->file);

    // open the file
    if (!this->file->Open(this->filename, "rb"))
    {
        n_printf("nNvx2Loader: could not open file '%s'!\n", this->filename.Get());
        this->Close();
        return false;
    }

    // read file header, including groups
    int magic = file->GetInt();
    if (magic != 'NVX2')
    {
        n_printf("nNvx2Loader: '%s' is not a NVX2 file!\n", this->filename.Get());
        this->Close();
        return false;
    }
    this->numGroups        = file->GetInt();
    this->numVertices      = file->GetInt();
    this->fileVertexWidth  = file->GetInt();
    this->numTriangles     = file->GetInt();
    this->numEdges         = file->GetInt();
    this->fileVertexComponents = file->GetInt();
    this->numIndices       = this->numTriangles * 3;

    int groupIndex;
    for (groupIndex = 0; groupIndex < this->numGroups; groupIndex++)
    {
        int firstVertex   = file->GetInt();
        int numVertices   = file->GetInt();
        int firstTriangle = file->GetInt();
        int numTriangles  = file->GetInt();
        int firstEdge     = file->GetInt();
        int numEdges      = file->GetInt();

        nMeshGroup group;
        group.SetFirstVertex(firstVertex);
        group.SetNumVertices(numVertices);
        group.SetFirstIndex(firstTriangle * 3);
        group.SetNumIndices(numTriangles * 3);
        group.SetFirstEdge(firstEdge);
        group.SetNumEdges(numEdges);
        this->groupArray.Append(group);
    }

    return nMeshLoader::Open();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nNvx2Loader::Close()
{
    if (this->file)
    {
        if (this->file->IsOpen())
        {
            this->file->Close();
        }
        this->file->Release();
        this->file = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nNvx2Loader::ReadVertices(void* buffer, int bufferSize)
{
    n_assert(buffer);
    n_assert(this->file);
    n_assert((this->numVertices * this->vertexWidth * int(sizeof(float))) == bufferSize);
    if (this->vertexComponents == this->fileVertexComponents)
    {
        n_assert(this->vertexWidth == this->fileVertexWidth);
        file->Read(buffer, bufferSize);
    }
    else
    {
        float* destBuf = (float*) buffer;
        float* readBuffer = n_new_array(float, this->fileVertexWidth);
        const int readSize = int(sizeof(float)) * this->fileVertexWidth;
        int v = 0;
        for (v = 0; v < this->numVertices; v++)
        {
            float* vBuf = readBuffer;
            int numRead = file->Read(vBuf, readSize);
            n_assert(numRead == readSize);

            int bitIndex;
            for (bitIndex = 0; bitIndex < nMesh2::NumVertexComponents; bitIndex++)
            {
                int mask = (1<<bitIndex);

                // skip completely if current vertex component is not in file
                if (0 == (this->fileVertexComponents & mask))
                {
                    continue;
                }

                // get width of current vertex component
                int width = nMesh2::GetVertexWidthFromMask(mask);
                n_assert(width > 0);
                if (this->vertexComponents & mask)
                {
                    // read the vertex component
                    int f;
                    for (f = 0; f < width; f++)
                    {
                        *destBuf++ = *vBuf++;
                    }
                }
                else
                {
                    // skip the vertex component
                    vBuf += width;
                }
            }
        }
        n_delete_array(readBuffer);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nNvx2Loader::ReadIndices(void* buffer, int bufferSize)
{
    n_assert(buffer);
    n_assert(this->file);
    if (Index16 == this->indexType)
    {
        // 16 bit indices: read index array directly
        n_assert((this->numIndices * int(sizeof(ushort))) == bufferSize);
        file->Read(buffer, bufferSize);
    }
    else
    {
        // 32 bit indices, read into 16 bit buffer, and expand
        n_assert((this->numIndices * int(sizeof(uint))) == bufferSize);

        // read 16 bit indices into tmp buffer
        int size16 = this->numIndices * sizeof(ushort);
        ushort* ptr16 = (ushort*)n_malloc(size16);
        n_assert(ptr16);
        file->Read(ptr16, size16);

        // expand to 32 bit indices
        uint* ptr32 = (uint*) buffer;
        int i;
        for (i = 0; i < this->numIndices; i++)
        {
            ptr32[i] = (uint)ptr16[i];
        }

        // release tmp buffer
        n_free(ptr16);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    A edge has the size of 4 * ushort, so you have to provide a buffer with the
    size numEdges * 4 * sizeof(ushort).
    The edge data is: ushort faceIndex1, faceIndex2, vertexIndex1, vertexIndex2;
    If a face Indicie is invalid (a border edge with only on face connected)
    the value is (ushort)nMeshBuilder::InvalidIndex (== -1).
*/
inline
bool
nNvx2Loader::ReadEdges(void* buffer, int bufferSize)
{
    n_assert(buffer);
    n_assert(this->file);
    if (this->numEdges > 0)
    {
        n_assert((this->numEdges * 4 * int(sizeof(ushort))) == bufferSize);
        file->Read(buffer, bufferSize);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
#endif
