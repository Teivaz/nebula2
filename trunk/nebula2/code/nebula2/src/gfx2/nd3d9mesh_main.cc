//------------------------------------------------------------------------------
//  nd3d9mesh_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nd3d9mesh.h"

nNebulaClass(nD3D9Mesh, "nmesh2");

//------------------------------------------------------------------------------
/**
*/
nD3D9Mesh::nD3D9Mesh() :
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
    if (this->IsLoaded())
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
    n_assert(!this->IsLoaded());
    n_assert(0 == this->vertexBuffer);
    n_assert(0 == this->indexBuffer);

    bool success = nMesh2::LoadResource();

    if (success)
    {
        // create the vertex declaration from the vertex component mask
        this->CreateVertexDeclaration();
    }
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
    n_assert(this->IsLoaded());

    nMesh2::UnloadResource();

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

    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device is lost. We only need to
    react if our vertex and index buffers are not in D3D's managed pool.
    In this case, we need to unload ourselves...
*/
void
nD3D9Mesh::OnLost()
{
    if (WriteOnly & this->vertexUsage)
    {
        this->UnloadResource();
        this->SetState(Lost);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the d3d device has been restored. If our
    buffers are in the D3D's default pool, we need to restore ourselves
    as well, and we need to set our state to empty, because the buffers contain
    no data.
*/
void
nD3D9Mesh::OnRestored()
{
    if (WriteOnly & this->vertexUsage)
    {
        this->SetState(Unloaded);
        this->LoadResource();
        this->SetState(Empty);
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d vertex buffer and validate the vertexBuffer member.

    - 27-Sep-04     floh    DX7 compatibility fix: software processing meshes
                            now created in system memory, and without the WRITEONLY flag
*/
void
nD3D9Mesh::CreateVertexBuffer()
{
    n_assert(this->vertexBufferByteSize > 0);
    n_assert(0 == this->privVertexBuffer);
    n_assert(0 == this->vertexBuffer);
    n_assert(this->vertexBufferByteSize > 0);

    if (ReadOnly & this->vertexUsage)
    {
        // this is a read-only mesh which will never be rendered
        // and only read-accessed by the CPU, allocate private
        // vertex buffer
        this->privVertexBuffer = n_malloc(this->vertexBufferByteSize);
        n_assert(this->privVertexBuffer);
    }
    else
    {
        nD3D9Server* gfxServer = (nD3D9Server*)nGfxServer2::Instance();
        n_assert(gfxServer->d3d9Device);

        // this is either a WriteOnce or a WriteOnly vertex buffer,
        // in both cases we create a D3D vertex buffer object
        HRESULT hr;

        DWORD d3dUsage = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool = D3DPOOL_MANAGED;
        this->d3dVBLockFlags = 0;
        if (WriteOnly & this->vertexUsage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dVBLockFlags = D3DLOCK_DISCARD;
        }
        if (ReadWrite & this->vertexUsage)
        {
            d3dUsage = D3DUSAGE_DYNAMIC;
            d3dPool  = D3DPOOL_SYSTEMMEM;
        }
        if (RTPatch & this->vertexUsage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->vertexUsage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }

        // create buffer with software processing flag
        // if the NeedsVertexShader hint is enabled, and the d3d device
        // has been created with software or mixed vertex processing
        if (gfxServer->AreVertexShadersEmulated() && (NeedsVertexShader & this->vertexUsage))
        {
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
            d3dUsage &= ~D3DUSAGE_WRITEONLY;
            d3dPool = D3DPOOL_SYSTEMMEM;
        }

        // create the vertex buffer
        hr = gfxServer->d3d9Device->CreateVertexBuffer(
                this->vertexBufferByteSize,
                d3dUsage,
                0,
                d3dPool,
                &(this->vertexBuffer),
                NULL);
        n_dxtrace(hr, "CreateVertexBuffer() failed in nD3D9Mesh");
        n_assert(this->vertexBuffer);
    }
}

//------------------------------------------------------------------------------
/**
    Create a static d3d index buffer and validate the index buffer member.

    - 27-Sep-04     floh    DX7 compatibility fix: software processing meshes
                            now created in system mem, and without the WRITEONLY flag
*/
void
nD3D9Mesh::CreateIndexBuffer()
{
    n_assert(this->indexBufferByteSize > 0);
    n_assert(0 == this->indexBuffer);
    n_assert(0 == this->privIndexBuffer);
    n_assert(this->indexBufferByteSize > 0);

    if (ReadOnly & this->indexUsage)
    {
        this->privIndexBuffer = n_malloc(this->indexBufferByteSize);
        n_assert(this->privIndexBuffer);
    }
    else
    {
        nD3D9Server* gfxServer = (nD3D9Server*)nGfxServer2::Instance();
        n_assert(gfxServer->d3d9Device);
        HRESULT hr;

        DWORD d3dUsage       = D3DUSAGE_WRITEONLY;
        D3DPOOL d3dPool      = D3DPOOL_MANAGED;
        this->d3dIBLockFlags = 0;
        if (WriteOnly & this->indexUsage)
        {
            d3dUsage = (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
            d3dPool  = D3DPOOL_DEFAULT;
            this->d3dIBLockFlags = D3DLOCK_DISCARD;
        }
        if (ReadWrite & this->indexUsage)
        {
            d3dUsage = D3DUSAGE_DYNAMIC;
            d3dPool  = D3DPOOL_SYSTEMMEM;
        }
        if (RTPatch & this->indexUsage)
        {
            d3dUsage |= D3DUSAGE_RTPATCHES;
        }
        if (PointSprite & this->indexUsage)
        {
            d3dUsage |= D3DUSAGE_POINTS;
        }
        // create buffer with software processing flag
        // if the NeedsVertexShader hint is enabled, and the d3d device
        // has been created with software or mixed vertex processing
        if (gfxServer->AreVertexShadersEmulated() && (NeedsVertexShader & this->vertexUsage))
        {
            d3dUsage |= D3DUSAGE_SOFTWAREPROCESSING;
            d3dUsage &= ~D3DUSAGE_WRITEONLY;
            d3dPool = D3DPOOL_SYSTEMMEM;
        }

        hr = gfxServer->d3d9Device->CreateIndexBuffer(
                this->indexBufferByteSize,
                d3dUsage,
                D3DFMT_INDEX16,
                d3dPool,
                &(this->indexBuffer),
                NULL);
        n_dxtrace(hr, "CreateIndexBuffer failed in nD3D9Mesh");
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
    nD3D9Server* gfxServer = (nD3D9Server*) nGfxServer2::Instance();
    n_assert(gfxServer->d3d9Device);

    const int maxElements = NumVertexComponents;
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

                case Coord4:
                    decl[curElement].Type       = D3DDECLTYPE_FLOAT4;
                    decl[curElement].Usage      = D3DDECLUSAGE_POSITION;
                    decl[curElement].UsageIndex = 0;
                    curOffset += 4 * sizeof(float);
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

    HRESULT hr = gfxServer->d3d9Device->CreateVertexDeclaration(decl, &(this->vertexDeclaration));
    n_dxtrace(hr, "CreateVertexDeclaration() failed in nD3D9Mesh");
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
        n_dxtrace(hr, "Lock() on vertex buffer failed in nD3D9Mesh()");
        n_assert(ptr);
        retval = (float*)ptr;
    }
    else
    {
        retval = (float*)this->privVertexBuffer;
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
        HRESULT hr = this->vertexBuffer->Unlock();
        n_dxtrace(hr, "Unlock() on vertex buffer failed in nD3D9Mesh");
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
    this->LockMutex();
    n_assert(this->indexBuffer || this->privIndexBuffer);
    ushort* retval = 0;
    if (this->indexBuffer)
    {
        VOID* ptr;
        HRESULT hr = this->indexBuffer->Lock(0, 0, &ptr, this->d3dIBLockFlags);
        n_dxtrace(hr, "Lock on index buffer failed in nD3D9Mesh");
        n_assert(ptr);
        retval = (ushort*)ptr;
    }
    else
    {
        retval = (ushort*)this->privIndexBuffer;
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
        n_dxtrace(hr, "Unlock() on index buffer failed in nD3D9Mesh");
    }
    this->UnlockMutex();
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
        int indexBufferSize  = this->GetNumIndices() * sizeof(ushort);
        return vertexBufferSize + indexBufferSize + nMesh2::GetByteSize();
    }
    return 0;
}
