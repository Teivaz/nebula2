#define N_IMPLEMENTS nD3D9Mesh
//------------------------------------------------------------------------------
//  nd3d9mesh_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9mesh.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaClass(nD3D9Mesh, "nmesh2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Mesh::nD3D9Mesh() :
    refGfxServer("/sys/servers/gfx"),
    d3dVBLockFlags(0),
    d3dIBLockFlags(0),
    vertexBuffer(0),
    indexBuffer(0),
    vertexDeclaration(0),
    privVertexBuffer(0),
    privIndexBuffer(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nD3D9Mesh::~nD3D9Mesh()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Load mesh data from resource file and create a D3D vertex buffer and
    index buffer.
*/
bool
nD3D9Mesh::Load()
{
    n_assert(!this->valid);
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    if (this->filename.IsEmpty())
    {
        // no filename, just create empty vertex and index buffers        
        int verticesByteSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        int indicesByteSize  = this->GetNumIndices() * sizeof(ushort);
        this->SetVertexBufferByteSize(verticesByteSize);
        this->SetIndexBufferByteSize(indicesByteSize);
        this->CreateVertexBuffer();
        this->CreateIndexBuffer();
        this->CreateVertexDeclaration();
        this->valid = true;
    }
    // This is placed AFTER IsEmpty(), because the IsEmpty() check is our hack-ish way of getting
    // the mesh to create the vertex and index buffers for us for nResourceLoader.
    else if (refResourceLoader.isvalid())
    {
        // if the resource loader reference is valid, let it take a stab at the file
        this->valid = refResourceLoader->Load(this->filename.Get(), this);
    }
    else if (this->filename.CheckExtension("nvx2"))
    {
        // load from nvx2 file
        this->valid = this->LoadNvx2File();
    }
    else if (this->filename.CheckExtension("n3d2"))
    {
        // load from n3d2 file
        this->valid = this->LoadN3d2File();
    }
    else
    {
        n_printf("nD3D9Mesh: file extension not recognized (must be .nvx2 or .n3d2)\n");
        this->valid = false;
    }

    // fail hard if loading failed
    if (!this->valid)
    {
        n_error("Could not open mesh '%s'!\n", this->filename.Get());
    }

    return this->valid;
}

//------------------------------------------------------------------------------
/**
    Unload the d3d resources. Make sure that the resource are properly
    disconnected from the graphics server.
*/
void
nD3D9Mesh::Unload()
{
    if (this->valid)
    {
        nMesh2::Unload();

        nD3D9Server* gfxServer = this->refGfxServer.get();
        n_assert(gfxServer->d3d9Device);

        // check if I'm the current mesh in the gfx server, if yes, unlink
        int curStream;
        for (curStream = 0; curStream < nGfxServer2::MAX_VERTEXSTREAMS; curStream++)
        {
            if (gfxServer->GetMesh(curStream) == this)
            {
                gfxServer->SetMesh(curStream, 0);
            }
        }

        // release the d3d resource
        if (this->vertexBuffer)
        {
            this->vertexBuffer->Release();
            this->vertexBuffer = 0;
        }
        if (this->indexBuffer)
        {
            this->indexBuffer->Release();
            this->indexBuffer = 0;
        }
        if (this->vertexDeclaration)
        {
            this->vertexDeclaration->Release();
            this->vertexDeclaration = 0;
        }

        // release private buffers (if this is a ReadOnly mesh)
        if (this->privVertexBuffer)
        {
            n_free(this->privVertexBuffer);
            this->privVertexBuffer = 0;
        }
        if (this->privIndexBuffer)
        {
            n_free(this->privIndexBuffer);
            this->privIndexBuffer = 0;
        }

        this->valid = false;
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d vertex buffer and validate the vertexBuffer member.

    @param  byteSize    size of vertex buffer in bytes
*/
void
nD3D9Mesh::CreateVertexBuffer()
{
    n_assert(this->vertexBufferByteSize > 0);
    n_assert(0 == this->privVertexBuffer);
    n_assert(0 == this->vertexBuffer);
    n_assert(this->vertexBufferByteSize > 0);

    if (ReadOnly == this->usage)
    {
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // vertex buffer
        this->privVertexBuffer = n_malloc(this->vertexBufferByteSize);
        n_assert(this->privVertexBuffer);
    }
    else
    {
        // this is either a WriteOnce or a WriteOnly vertex buffer,
        // in both cases we create a D3D vertex buffer object
        n_assert(this->refGfxServer->d3d9Device);
        HRESULT hr;    

        DWORD d3dUsage       = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool      = D3DPOOL_MANAGED;
        this->d3dVBLockFlags = D3DLOCK_NOSYSLOCK;
        if (WriteOnly == this->usage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dVBLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;
        }
        if (this->refGfxServer->GetSoftwareVertexProcessing())
        {
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
        }
        
        // create the vertex buffer
        hr = this->refGfxServer->d3d9Device->CreateVertexBuffer(
                this->vertexBufferByteSize,
                d3dUsage,
                0,
                d3dPool,
                &(this->vertexBuffer),
                NULL);
        n_assert(SUCCEEDED(hr));
        n_assert(this->vertexBuffer);
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d index buffer and validate the index buffer member.

    @param  byteSize    size of index buffer in bytes
*/
void
nD3D9Mesh::CreateIndexBuffer()
{
    n_assert(this->indexBufferByteSize > 0);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->privIndexBuffer);
    n_assert(this->indexBufferByteSize > 0);

    if (ReadOnly == this->usage)
    {
        this->privIndexBuffer = n_malloc(this->indexBufferByteSize);
        n_assert(this->privIndexBuffer);
    }
    else
    {
        n_assert(this->refGfxServer->d3d9Device);
        HRESULT hr;

        DWORD d3dUsage       = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool      = D3DPOOL_MANAGED;
        this->d3dIBLockFlags = D3DLOCK_NOSYSLOCK;
        if (WriteOnly == this->usage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dIBLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;
        }

        hr = this->refGfxServer->d3d9Device->CreateIndexBuffer(
                this->indexBufferByteSize,
                d3dUsage,
                D3DFMT_INDEX16,
                d3dPool,
                &(this->indexBuffer),
                NULL);
        n_assert(SUCCEEDED(hr));
        n_assert(this->indexBuffer);
    }
}

//------------------------------------------------------------------------------
/**
    Create the d3d9 vertex declaration from the vertex component mask.
*/
void
nD3D9Mesh::CreateVertexDeclaration()
{
    n_assert(0 == this->vertexDeclaration);
    n_assert(this->refGfxServer->d3d9Device);

    const int maxElements = 11;
    D3DVERTEXELEMENT9 decl[maxElements];
    int curElement = 0;
    int curOffset  = 0;
    int index;
    for (index = 0; index < maxElements; index++)
    {
        int mask = (1<<index);
        if (this->vertexComponentMask & mask)
        {
            decl[curElement].Stream = 0;
            decl[curElement].Offset = curOffset;
            decl[curElement].Method = D3DDECLMETHOD_DEFAULT;
            switch (mask)
            {
                case Coord:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                    decl[curElement].Usage      = D3DDECLUSAGE_POSITION;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 3 * sizeof(float);
                    break;

                case Normal:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                    decl[curElement].Usage      = D3DDECLUSAGE_NORMAL;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 3 * sizeof(float);
                    break;

                case Tangent:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                    decl[curElement].Usage      = D3DDECLUSAGE_TANGENT;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 3 * sizeof(float);
                    break;

                case Binormal:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT3;
                    decl[curElement].Usage      = D3DDECLUSAGE_BINORMAL;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 3 * sizeof(float);
                    break;

                case Color:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                    decl[curElement].Usage      = D3DDECLUSAGE_COLOR;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 4 * sizeof(float);
                    break;

                case Uv0:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                    decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 2 * sizeof(float);
                    break;

                case Uv1:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                    decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                    decl[curElement].UsageIndex = 1;
                    curOffset += 2 * sizeof(float);
                    break;

                case Uv2:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                    decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                    decl[curElement].UsageIndex = 2;
                    curOffset += 2 * sizeof(float);
                    break;

                case Uv3:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT2;
                    decl[curElement].Usage      = D3DDECLUSAGE_TEXCOORD;
                    decl[curElement].UsageIndex = 3;
                    curOffset += 2 * sizeof(float);
                    break;

                case Weights:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                    decl[curElement].Usage      = D3DDECLUSAGE_BLENDWEIGHT;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 4 * sizeof(float);
                    break;

                case JIndices:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                    decl[curElement].Usage      = D3DDECLUSAGE_BLENDINDICES;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 4 * sizeof(float);
                    break;

                default:
                    n_error("Unknown vertex component in vertex component mask");
                    break;
            }
            curElement++;
        }
    }

    // write vertex declaration terminator element, see D3DDECL_END() macro in d3d9types.h for details
    decl[curElement].Stream = 0xff;
    decl[curElement].Offset = 0;
    decl[curElement].Type   = D3DDECLTYPE_UNUSED;
    decl[curElement].Method = 0;
    decl[curElement].Usage  = 0;
    decl[curElement].UsageIndex = 0;

    HRESULT hr = this->refGfxServer->d3d9Device->CreateVertexDeclaration(decl, &(this->vertexDeclaration));
    n_assert(SUCCEEDED(hr));
    n_assert(this->vertexDeclaration);
}

//------------------------------------------------------------------------------
/**
    Lock the d3d vertex buffer and return pointer to it.
*/
float*
nD3D9Mesh::LockVertices()
{
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    if (this->vertexBuffer)
    {
        VOID* ptr;
        HRESULT hr = this->vertexBuffer->Lock(0, 0, &ptr, this->d3dVBLockFlags);
        n_assert(SUCCEEDED(hr));
        n_assert(ptr);
        return (float*) ptr;
    }
    else
    {
        return (float*) this->privVertexBuffer;
    }
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d vertex buffer locked by nD3D9Mesh::LockVertices().
*/
void
nD3D9Mesh::UnlockVertices()
{
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    if (this->vertexBuffer)
    {
        HRESULT hr;
        hr = this->vertexBuffer->Unlock();
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
    Lock the d3d index buffer and return pointer to it.
*/
ushort*
nD3D9Mesh::LockIndices()
{
    n_assert(this->indexBuffer || this->privIndexBuffer);
    if (this->indexBuffer)
    {
        VOID* ptr;
        HRESULT hr = this->indexBuffer->Lock(0, 0, &ptr, this->d3dIBLockFlags);
        n_assert(SUCCEEDED(hr));
        n_assert(ptr);
        return (ushort*) ptr;
    }
    else
    {
        return (ushort*) this->privIndexBuffer;
    }
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d index buffer locked by nD3D9Mesh::LockIndices().
*/
void
nD3D9Mesh::UnlockIndices()
{
    n_assert(this->indexBuffer || this->privIndexBuffer);
    if (this->indexBuffer)
    {
        HRESULT hr = this->indexBuffer->Unlock();
        n_assert(SUCCEEDED(hr));
    }
}

//------------------------------------------------------------------------------
/**
    Update the group bounding boxes. This is a slow operation (since the
    d3d vertex buffer must be locked and read). It should only be called
    once after loading.
*/
void
nD3D9Mesh::UpdateGroupBoundingBoxes()
{
    bbox3 groupBox;
    int groupIndex;

    float* vertexBufferData = this->LockVertices();
    ushort* indexBufferData = this->LockIndices();
    for (groupIndex = 0; groupIndex < this->numGroups; groupIndex++)
    {
        groupBox.begin_extend();

        const nMeshGroup& group = this->GetGroup(groupIndex);
        ushort* indexPointer = indexBufferData + group.GetFirstIndex();
        int i;
        for (i = 0; i < group.GetNumIndices(); i++)
        {
            float* vertexPointer = vertexBufferData + (indexPointer[i] * this->vertexWidth);
            groupBox.extend(vertexPointer[0], vertexPointer[1], vertexPointer[2]);
        }
    }
    this->UnlockIndices();
    this->UnlockVertices();
}

//------------------------------------------------------------------------------
/**
    Read an .nvx2 file (Nebula's binary mesh file format).
*/
bool
nD3D9Mesh::LoadNvx2File()
{
    n_assert(!this->valid);

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);

    // open the file
    if (!file->Open(this->filename.Get(), "rb"))
    {
        n_printf("nD3D9Mesh: could not open file '%s'!\n", this->filename.Get());
        delete file;
        return false;
    }

    // read header
    int magic;
    int numGrps, numVerts, numTris, vertexComponents;
    int vertWidth;

    file->GetInt(magic);
    if (magic != 'NVX2')
    {
        n_printf("nD3D9Mesh: '%s' is not a NVX2 file!\n", this->filename.Get());
        file->Close();
        delete file;
        return false;
    }
    file->GetInt(numGrps);
    file->GetInt(numVerts);
    file->GetInt(vertWidth);
    file->GetInt(numTris);
    file->GetInt(vertexComponents);

    // set nMesh2 attributes
    this->SetNumGroups(numGrps);
    this->SetNumVertices(numVerts);
    this->SetNumIndices(numTris * 3);
    this->SetVertexComponents(vertexComponents);
    this->SetPrimitiveType(TRIANGLELIST);

    // get data chunk sizes
    int verticesByteSize = numVerts * vertWidth * sizeof(float);
    int indicesByteSize  = numTris * 3 * sizeof(ushort);
    this->SetVertexBufferByteSize(verticesByteSize);
    this->SetIndexBufferByteSize(indicesByteSize);

    // read the group information...
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGrps; groupIndex++)
    {
        int firstVertex, numVertices, firstTriangle, numTriangles;
        file->GetInt(firstVertex);
        file->GetInt(numVertices);
        file->GetInt(firstTriangle);
        file->GetInt(numTriangles);

        nMeshGroup& group = this->GetGroup(groupIndex);
        group.SetFirstVertex(firstVertex);
        group.SetNumVertices(numVertices);
        group.SetFirstIndex(firstTriangle * 3);
        group.SetNumIndices(numTriangles * 3);
    }

    // create the d3d vertex and index buffers
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // read vertex data and update the group's bounding boxes
    float* vertexBufferData = this->LockVertices();
    n_assert(vertexBufferData);
    file->Read(vertexBufferData, verticesByteSize);
    this->UnlockVertices();

    // fill the index data
    ushort* indexBufferData = this->LockIndices();
    file->Read(indexBufferData, indicesByteSize);
    this->UnlockIndices();

    // update group bounding boxes
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    this->CreateVertexDeclaration();

    // cleanup
    file->Close();
    delete file;

    return true;
}

//------------------------------------------------------------------------------
/**
    Read an .n3d2 file (Nebula ascii mesh file format).
*/
bool 
nD3D9Mesh::LoadN3d2File()
{
    n_assert(!this->valid);

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);

    // open the file
    if (!file->Open(this->filename.Get(), "r"))
    {
        n_printf("nD3D9Mesh: could not open file '%s'!\n", this->filename.Get());
        delete file;
        return false;
    }

    // read file line by line
    char line[1024];
    int curGroup = 0;
    int curVertex = 0;
    int curTriangle = 0;
    float* vertexBufferPtr = 0;
    ushort* indexBufferPtr = 0;
    while (file->GetS(line, sizeof(line)))
    {
        // get keyword
        char* keyWord = strtok(line, N_WHITESPACE);
        if (0 == keyWord)
        {
            continue;
        }
        else if (0 == strcmp(keyWord, "type"))
        {
            // type must be 'n3d2'
            const char* typeString = strtok(0, N_WHITESPACE);
            n_assert(typeString);
            if (0 != strcmp(typeString, "n3d2"))
            {
                n_printf("nD3D9Mesh::Load(%s): Invalid type '%s', must be 'n3d2'\n", filename.Get(), typeString);
                file->Close();
                delete file;
                return false;
            }
        }
        else if (0 == strcmp(keyWord, "numgroups"))
        {
            // number of groups
            const char* numGroupsString = strtok(0, N_WHITESPACE);
            n_assert(numGroupsString);
            this->SetNumGroups(atoi(numGroupsString));
        }
        else if (0 == strcmp(keyWord, "numvertices"))
        {
            // number of vertices
            const char* numVerticesString = strtok(0, N_WHITESPACE);
            n_assert(numVerticesString);
            this->SetNumVertices(atoi(numVerticesString));
        }
        else if (0 == strcmp(keyWord, "vertexcomps"))
        {
            char* str;
            int vertexComps = 0;
            while (str = strtok(0, N_WHITESPACE))
            {
                    if (0 == strcmp(str, "coord"))          vertexComps |= Coord;
                    else if (0 == strcmp(str, "normal"))    vertexComps |= Normal;
                    else if (0 == strcmp(str, "tangent"))   vertexComps |= Tangent;
                    else if (0 == strcmp(str, "binormal"))  vertexComps |= Binormal;
                    else if (0 == strcmp(str, "color"))     vertexComps |= Color;
                    else if (0 == strcmp(str, "uv0"))       vertexComps |= Uv0;
                    else if (0 == strcmp(str, "uv1"))       vertexComps |= Uv1;
                    else if (0 == strcmp(str, "uv2"))       vertexComps |= Uv2;
                    else if (0 == strcmp(str, "uv3"))       vertexComps |= Uv3;
                    else if (0 == strcmp(str, "weights"))   vertexComps |= Weights;
                    else if (0 == strcmp(str, "jindices"))  vertexComps |= JIndices;
            }
            this->SetVertexComponents(vertexComps);

            // we can now allocate the vertex buffer
            int vbSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
            this->SetVertexBufferByteSize(vbSize);
            this->CreateVertexBuffer();
            
            vertexBufferPtr = this->LockVertices();
            n_assert(vertexBufferPtr);
        }
        else if (0 == strcmp(keyWord, "numtris"))
        {
            // number of triangles
            const char* numTrisString = strtok(0, N_WHITESPACE);
            n_assert(numTrisString);
            this->SetNumIndices(atoi(numTrisString) * 3);

            // we can now allocate the index buffer
            int ibSize = this->GetNumIndices() * sizeof(ushort);
            this->SetIndexBufferByteSize(ibSize);
            this->CreateIndexBuffer();
            
            indexBufferPtr = this->LockIndices();
            n_assert(indexBufferPtr);

        }
        else if (0 == strcmp(keyWord, "g"))
        {
            // a group definition
            const char* firstVertString = strtok(0, N_WHITESPACE);
            const char* numVertsString  = strtok(0, N_WHITESPACE);
            const char* firstTriString  = strtok(0, N_WHITESPACE);
            const char* numTrisString   = strtok(0, N_WHITESPACE);
            
            n_assert(firstVertString && numVertsString);
            n_assert(firstTriString && numTrisString);
            
            nMeshGroup& meshGroup = this->GetGroup(curGroup++);
            meshGroup.SetFirstVertex(atoi(firstVertString));
            meshGroup.SetNumVertices(atoi(numVertsString));
            meshGroup.SetFirstIndex(atoi(firstTriString) * 3);
            meshGroup.SetNumIndices(atoi(numTrisString) * 3);
        }
        else if (0 == strcmp(keyWord, "v"))
        {
            // a vertex definition
            int curIndex;
            n_assert(vertexBufferPtr);
            float* vPtr = vertexBufferPtr + (curVertex * this->vertexWidth);
            for (curIndex = 0; curIndex < this->vertexWidth; curIndex++)
            {
                const char* curFloatString = strtok(0, N_WHITESPACE);
                n_assert(curFloatString);
                float curFloat = (float) atof(curFloatString);
                vPtr[curIndex] = curFloat;
            }
            curVertex++;
        }
        else if (0 == strcmp(keyWord, "t"))
        {
            // a triangle definition
            const char* i0String = strtok(0, N_WHITESPACE);
            const char* i1String = strtok(0, N_WHITESPACE);
            const char* i2String = strtok(0, N_WHITESPACE);
            n_assert(i0String && i1String && i2String);
            int i0 = atoi(i0String);
            int i1 = atoi(i1String);
            int i2 = atoi(i2String);

            n_assert(indexBufferPtr);
            ushort* iPtr = indexBufferPtr + (curTriangle * 3);
                
            iPtr[0] = (ushort) i0;
            iPtr[1] = (ushort) i1;
            iPtr[2] = (ushort) i2;

            curTriangle++;
        }
    }

    // unlock buffers
    if (vertexBufferPtr)
    {
        this->UnlockVertices();
    }
    if (indexBufferPtr)
    {
        this->UnlockIndices();
    }

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    this->CreateVertexDeclaration();

    // cleanup
    file->Close();
    delete file;

    return true;
}
