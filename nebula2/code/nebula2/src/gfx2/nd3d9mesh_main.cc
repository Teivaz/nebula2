//------------------------------------------------------------------------------
//  nd3d9mesh_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9mesh.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"

nNebulaClass(nD3D9Mesh, "nmesh2");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nd3d9mesh

    @cppclass
    nD3D9Mesh
    
    @superclass
    nmesh2
    
    @classinfo
    Docs needed.
*/

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
    nD3D9Mesh support asynchronous resource loading.
*/
bool
nD3D9Mesh::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    This method is either called directly from the nResource::Load() method
    (in synchronous mode), or from the loader thread (in asynchronous mode).
    The method must try to validate its resources, set the valid and pending
    flags, and return a success code.
    This method may be called from a thread.
*/
bool
nD3D9Mesh::LoadResource()
{
    n_assert(!this->IsValid());
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    nPathString filename(this->GetFilename().Get());
    bool success = false;
    if (filename.IsEmpty())
    {
        // no filename, just create empty vertex and index buffers        
        int verticesByteSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        int indicesByteSize  = this->GetNumIndices() * sizeof(ushort);
        this->SetVertexBufferByteSize(verticesByteSize);
        this->SetIndexBufferByteSize(indicesByteSize);
        this->CreateVertexBuffer();
        this->CreateIndexBuffer();
        this->CreateVertexDeclaration();
        switch (this->refillBuffersMode)
        {
            case DisabledOnce:
                this->refillBuffersMode = Enabled;
                break;
            case Enabled:
                this->refillBuffersMode = NeededNow;
                break;
            default:
                break;
        }
        success = true;
    }
    else if (this->refResourceLoader.isvalid())
    {
        // if the resource loader reference is valid, let it take a stab at the file
        success = this->refResourceLoader->Load(filename.Get(), this);
    }
    else if (filename.CheckExtension("nvx2"))
    {
        // load from nvx2 file
        success = this->LoadNvx2File();
    }
    else if (filename.CheckExtension("n3d2"))
    {
        // load from n3d2 file
        success = this->LoadN3d2File();
    }
    this->SetValid(success);
    return success;
}

//------------------------------------------------------------------------------
/**
    Unload the d3d resources. Make sure that the resource are properly
    disconnected from the graphics server. This method is called from
    nResource::Unload() which serves as a wrapper for synchronous and
    asynchronous mode. This method will NEVER be called from a thread
    though.
*/
void
nD3D9Mesh::UnloadResource()
{
    n_assert(this->IsValid());

    nMesh2::UnloadResource();

    nD3D9Server* gfxServer = this->refGfxServer.get();
    n_assert(gfxServer->d3d9Device);

    // check if I'm the current mesh in the gfx server, if yes, unlink
    if (gfxServer->GetMesh() == this)
    {
        gfxServer->SetMesh(0);
    }
/*FIXME: commentted out until we get correct version of GetMeshArray
    else
    {
        nMeshArray* meshArray = gfxServer->GetMeshArray();
        if (0 != meshArray)
        {
            int index = meshArray->ContainsMesh(this);
            if (index != -1)
        {
                meshArray->SetMesh(index, 0);
            }
        }
    }
*/

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

    this->SetValid(false);
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

    if (ReadOnly & this->usage)
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
        this->d3dVBLockFlags = 0;
        if (WriteOnly & this->usage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dVBLockFlags = D3DLOCK_DISCARD;
        }
        if (RTPatch & this->usage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->usage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }
        if (this->refGfxServer->GetSoftwareVertexProcessing() || 
            ((NeedsVertexShader & this->usage) && 
            (this->refGfxServer->GetFeatureSet() < nGfxServer2::DX9)))
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

    if (ReadOnly & this->usage)
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
        this->d3dIBLockFlags = 0;
        if (WriteOnly & this->usage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dIBLockFlags = D3DLOCK_DISCARD;
        }
        if (RTPatch & this->usage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->usage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }
        if (this->refGfxServer->GetSoftwareVertexProcessing() ||
            ((NeedsVertexShader & this->usage) && 
            (this->refGfxServer->GetFeatureSet() < nGfxServer2::DX9)))
        { 
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
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
    this->LockMutex();
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    float* retval = 0;
    if (this->vertexBuffer)
    {
        VOID* ptr;
        HRESULT hr = this->vertexBuffer->Lock(0, 0, &ptr, this->d3dVBLockFlags);
        n_assert(SUCCEEDED(hr));
        n_assert(ptr);
        retval = (float*) ptr;
    }
    else
    {
        retval = (float*) this->privVertexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d vertex buffer locked by nD3D9Mesh::LockVertices().
*/
void
nD3D9Mesh::UnlockVertices()
{
    this->LockMutex();
    n_assert(this->vertexBuffer || this->privVertexBuffer);
    if (this->vertexBuffer)
    {
        HRESULT hr;
        hr = this->vertexBuffer->Unlock();
        n_assert(SUCCEEDED(hr));
    }
    this->UnlockMutex();
}

//------------------------------------------------------------------------------
/**
    Lock the d3d index buffer and return pointer to it.
*/
ushort*
nD3D9Mesh::LockIndices()
{
    n_assert(this->indexBuffer || this->privIndexBuffer);
    this->LockMutex();
    ushort* retval = 0;
    if (this->indexBuffer)
    {
        VOID* ptr;
        HRESULT hr = this->indexBuffer->Lock(0, 0, &ptr, this->d3dIBLockFlags);
        n_assert(SUCCEEDED(hr));
        n_assert(ptr);
        retval = (ushort*) ptr;
    }
    else
    {
        retval = (ushort*) this->privIndexBuffer;
    }
    this->UnlockMutex();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock the d3d index buffer locked by nD3D9Mesh::LockIndices().
*/
void
nD3D9Mesh::UnlockIndices()
{
    this->LockMutex();
    n_assert(this->indexBuffer || this->privIndexBuffer);
    if (this->indexBuffer)
    {
        HRESULT hr = this->indexBuffer->Unlock();
        n_assert(SUCCEEDED(hr));
    }
    this->UnlockMutex();
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

        nMeshGroup& group = this->GetGroup(groupIndex);
        ushort* indexPointer = indexBufferData + group.GetFirstIndex();
        int i;
        for (i = 0; i < group.GetNumIndices(); i++)
        {
            float* vertexPointer = vertexBufferData + (indexPointer[i] * this->vertexWidth);
            groupBox.extend(vertexPointer[0], vertexPointer[1], vertexPointer[2]);
        }
        group.SetBoundingBox(groupBox);
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
    n_assert(!this->IsValid());
    bool res;
    nString filename = this->GetFilename();

    // configure a mesh loader and load header
    nNvx2Loader meshLoader;
    meshLoader.SetFilename(filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index16);
    if (!meshLoader.Open(kernelServer->GetFileServer()))
    {
        n_error("nD3D9Mesh: could not open file '%s'!\n", filename.Get());
        return false;
    }

    // transfer header data
    this->SetNumGroups(meshLoader.GetNumGroups());
    this->SetNumVertices(meshLoader.GetNumVertices());
    this->SetVertexComponents(meshLoader.GetVertexComponents());
    this->SetNumIndices(meshLoader.GetNumIndices());
    int groupIndex;
    int numGroups = meshLoader.GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshGroup& group = this->GetGroup(groupIndex);
        group = meshLoader.GetGroupAt(groupIndex);
    }
    n_assert(this->GetVertexWidth() == meshLoader.GetVertexWidth());

    // allocate vertex and index buffers
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(ushort);
    this->SetVertexBufferByteSize(vbSize);
    this->SetIndexBufferByteSize(ibSize);
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // load vertex buffer
    float* vertexBufferPtr = this->LockVertices();
    res = meshLoader.ReadVertices(vertexBufferPtr, vbSize);
    n_assert(res);
    this->UnlockVertices();

    // load indices
    ushort* indexBufferPtr = this->LockIndices();
    res = meshLoader.ReadIndices(indexBufferPtr, ibSize);
    n_assert(res);
    this->UnlockIndices();

    // close the meshloader
    meshLoader.Close();

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    this->CreateVertexDeclaration();

    return true;
}

//------------------------------------------------------------------------------
/**
    Read an .n3d2 file (Nebula ascii mesh file format).
*/
bool 
nD3D9Mesh::LoadN3d2File()
{
    n_assert(!this->IsValid());
    bool res;
    nString filename = this->GetFilename();

    // configure a mesh loader and load header
    nN3d2Loader meshLoader;
    meshLoader.SetFilename(filename.Get());
    meshLoader.SetIndexType(nMeshLoader::Index16);
    if (!meshLoader.Open(kernelServer->GetFileServer()))
    {
        n_error("nD3D9Mesh: could not open file '%s'!\n", filename.Get());
        return false;
    }

    // transfer header data
    this->SetNumGroups(meshLoader.GetNumGroups());
    this->SetNumVertices(meshLoader.GetNumVertices());
    this->SetVertexComponents(meshLoader.GetVertexComponents());
    this->SetNumIndices(meshLoader.GetNumIndices());
    int groupIndex;
    int numGroups = meshLoader.GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshGroup& group = this->GetGroup(groupIndex);
        group = meshLoader.GetGroupAt(groupIndex);
    }
    n_assert(this->GetVertexWidth() == meshLoader.GetVertexWidth());

    // allocate vertex and index buffers
    int vbSize = meshLoader.GetNumVertices() * meshLoader.GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader.GetNumIndices() * sizeof(ushort);
    this->SetVertexBufferByteSize(vbSize);
    this->SetIndexBufferByteSize(ibSize);
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // load vertex buffer
    float* vertexBufferPtr = this->LockVertices();
    res = meshLoader.ReadVertices(vertexBufferPtr, vbSize);
    n_assert(res);
    this->UnlockVertices();

    // load indices
    ushort* indexBufferPtr = this->LockIndices();
    res = meshLoader.ReadIndices(indexBufferPtr, ibSize);
    n_assert(res);
    this->UnlockIndices();

    // close the meshloader
    meshLoader.Close();

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    // create the vertex declaration from the vertex component mask
    this->CreateVertexDeclaration();

    return true;
}

//------------------------------------------------------------------------------
/**
    Compute the byte size of the mesh data
*/
int
nD3D9Mesh::GetByteSize()
{
    if (this->IsValid())
    {
        int vertexBufferSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        int indexBufferSize = this->GetNumIndices() * sizeof(ushort);
        return vertexBufferSize + indexBufferSize;
    }
    else
    {
        return 0;
    }
}
