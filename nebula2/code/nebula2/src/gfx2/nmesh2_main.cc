//------------------------------------------------------------------------------
//  nmesh2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nmesh2.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nn3d2loader.h"
#include "gfx2/nnvx2loader.h"
#include "gfx2/nmesharray.h"

nNebulaClass(nMesh2, "nresource");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nmesh2

    @cppclass
    nMesh2
    
    @superclass
    nresource
    
    @classinfo
    Docs needed.
*/
//------------------------------------------------------------------------------
/**
*/
nMesh2::nMesh2() :
    usage(WriteOnce),
    vertexComponentMask(0),
    vertexWidth(0),
    numVertices(0),
    numIndices(0),
    numEdges(0),
    numGroups(0),
    groups(0),
    vertexBufferByteSize(0),
    indexBufferByteSize(0),
    edgeBufferByteSize(0),
    privEdgeBuffer(0),
    refillBuffersMode(Disabled)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMesh2::~nMesh2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Unload everything.
*/
void
nMesh2::UnloadResource()
{
    n_assert(this->IsValid());
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(gfxServer);

    // check if I'm the current mesh in the gfx server, if yes, unlink
    if (gfxServer->GetMesh() == this)
    {
        gfxServer->SetMesh(0);
    }
    else
    {
        nMeshArray* meshArray = gfxServer->GetMeshArray();
        if (0 != meshArray)
        {
            int i;
            for(i = 0; i < nGfxServer2::MaxVertexStreams; i++)
            {
                nMesh2* mesh = meshArray->GetMeshAt(i);
                if (0 != mesh)
                {
                    if (this == mesh)
                    {
                        gfxServer->SetMeshArray(0);
                        break;
                    }
                }
            }
        }
    }

    if (this->groups)
    {
        delete[] this->groups;
        this->groups = 0;
    }

    // NOTE: do not clear numVertices, numIndices, vertexWidth, 
    // etc. Those values may be needed in a following call to Load()!
    this->vertexBufferByteSize = 0;
    this->indexBufferByteSize  = 0;
    this->edgeBufferByteSize = 0;

    // release private edge buffer
    if (this->privEdgeBuffer)
    {
        n_free(this->privEdgeBuffer);
        this->privEdgeBuffer = 0;
    }
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
nMesh2::LoadResource()
{
    n_assert(!this->IsValid());

    nPathString filename(this->GetFilename().Get());
    bool success = false;
    if (filename.IsEmpty())
    {
        // no filename, just create empty vertex and/or index buffers        
        success = CreateEmpty();
    }
    else if (this->refResourceLoader.isvalid())
    {
        // if the resource loader reference is valid, let it take a stab at the file
        success = this->refResourceLoader->Load(filename.Get(), this);
    }
    else
    {
        nMeshLoader* meshLoader = 0;

        // select meshloader
        if (filename.CheckExtension("nvx2"))
        {
            meshLoader = n_new nNvx2Loader;
        }
        else if (filename.CheckExtension("n3d2"))
        {
            meshLoader = n_new nN3d2Loader;
        }
        else
        {
            n_error("nMesh2::LoadResource: filetype not supported!\n");
        }

        if (0 != meshLoader)
        {
            success = this->LoadFile(meshLoader);
            n_delete meshLoader;
        }
    }
    this->SetValid(success);
    return success;
}

//------------------------------------------------------------------------------
/**
    Lock vertices in mesh. Return pointer to beginning of vertex buffer.
*/
float*
nMesh2::LockVertices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock vertices in mesh.
*/
void
nMesh2::UnlockVertices()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Lock indices in mesh. Return pointer to start of indices.
*/
ushort*
nMesh2::LockIndices()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
    Unlock indices in mesh.
*/
void
nMesh2::UnlockIndices()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Lock edges in mesh. Return pointer to start of edges.
*/
nMesh2::Edge*
nMesh2::LockEdges()
{
    n_assert(this->privEdgeBuffer);
    Edge* retval = this->privEdgeBuffer;
    return retval;
}

//------------------------------------------------------------------------------
/**
    Unlock edges in mesh.
*/
void
nMesh2::UnlockEdges()
{
    // no operation needed.
}

//------------------------------------------------------------------------------
/**
*/
void
nMesh2::CreateEdgeBuffer()
{
    n_assert(0 == this->privEdgeBuffer);
    n_assert(this->edgeBufferByteSize > 0);
    this->privEdgeBuffer = (Edge*) n_malloc(this->edgeBufferByteSize);
    n_assert(this->privEdgeBuffer);
}

//------------------------------------------------------------------------------
/**
    Use the provided nMeshLoader class to read the data.
    
    @param meshLoader   the meshloader to load the file.
*/
bool 
nMesh2::LoadFile(nMeshLoader* meshLoader)
{
    n_assert(meshLoader);
    n_assert(!this->IsValid());
    
    bool res;
    nString filename = this->GetFilename();

    // configure a mesh loader and load header
    meshLoader->SetFilename(filename.Get());
    meshLoader->SetIndexType(nMeshLoader::Index16);
    if (!meshLoader->Open(kernelServer->GetFileServer()))
    {
        n_error("nMesh2: could not open file '%s'!\n", filename.Get());
        return false;
    }

    // transfer header data
    this->SetNumGroups(meshLoader->GetNumGroups());
    this->SetNumVertices(meshLoader->GetNumVertices());
    this->SetVertexComponents(meshLoader->GetVertexComponents());
    this->SetNumIndices(meshLoader->GetNumIndices());
    this->SetNumEdges(meshLoader->GetNumEdges());
    
    int groupIndex;
    int numGroups = meshLoader->GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshGroup& group = this->GetGroup(groupIndex);
        group = meshLoader->GetGroupAt(groupIndex);
    }
    n_assert(this->GetVertexWidth() == meshLoader->GetVertexWidth());

    // allocate vertex and index buffers
    int vbSize = meshLoader->GetNumVertices() * meshLoader->GetVertexWidth() * sizeof(float);
    int ibSize = meshLoader->GetNumIndices() * sizeof(ushort);
    this->SetVertexBufferByteSize(vbSize);
    this->SetIndexBufferByteSize(ibSize);
    this->CreateVertexBuffer();
    this->CreateIndexBuffer();

    // load vertex buffer
    float* vertexBufferPtr = this->LockVertices();
    res = meshLoader->ReadVertices(vertexBufferPtr, vbSize);
    n_assert(res);
    this->UnlockVertices();

    // load indices
    ushort* indexBufferPtr = this->LockIndices();
    res = meshLoader->ReadIndices(indexBufferPtr, ibSize);
    n_assert(res);
    this->UnlockIndices();

    // if the file contains edges load them
    if (this->numEdges > 0)
    {
        int ebSize = this->numEdges * sizeof(Edge);
        this->SetEdgeBufferByteSize(ebSize);
        this->CreateEdgeBuffer();
        Edge* edgeBufferPtr = this->LockEdges();
        res = meshLoader->ReadEdges(edgeBufferPtr, ebSize);
        n_assert(res);
        this->UnlockEdges();
    }

    // close the meshloader
    meshLoader->Close();

    // update the group bounding box data
    this->UpdateGroupBoundingBoxes();

    return true;
}

//------------------------------------------------------------------------------
/**
    Update the group bounding boxes. This is a slow operation (since the
    vertex buffer must be locked and read). It should only be called
    once after loading.
*/
void
nMesh2::UpdateGroupBoundingBoxes()
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
    Compute the byte size of the mesh data
*/
int
nMesh2::GetByteSize()
{   
    if (this->IsValid())
    {
        return this->numEdges * sizeof(Edge);
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called to create uninitialized buffers, etc., for a mesh 
    that is not to be loaded from a file.  Can also be called by custom 
    resource loaders, to do the basic preinitialization.
*/
bool
nMesh2::CreateEmpty()
{
    if (this->GetNumVertices() > 0)
    {
        int verticesByteSize = this->GetNumVertices() * this->GetVertexWidth() * sizeof(float);
        this->SetVertexBufferByteSize(verticesByteSize);
        this->CreateVertexBuffer();
    }

    if (this->GetNumIndices() > 0)
    {
        int indicesByteSize  = this->GetNumIndices() * sizeof(ushort);
        this->SetIndexBufferByteSize(indicesByteSize);
        this->CreateIndexBuffer();
    }

    // load edges ?
    if (this->GetNumEdges() > 0)
    {
        int edgesByteSize = this->GetNumEdges() * sizeof(Edge);
        this->SetEdgeBufferByteSize(edgesByteSize);
        this->CreateEdgeBuffer();
    }
    
    switch (this->refillBuffersMode)
    {
        case RefillBuffersMode::DisabledOnce:
            this->refillBuffersMode = RefillBuffersMode::Enabled;
            break;
        case RefillBuffersMode::Enabled:
            this->refillBuffersMode = RefillBuffersMode::NeededNow;
            break;
        default:
            break;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Converts the flags into a string of the form
    "ReadOnly|NeedsVertexShader|"
    (includes a trailing '|')
*/

nString nMesh2::ConvertUsageFlagsToString( int usageFlags )
{
    nString usageString;
    if( usageFlags )
    {
        if( usageFlags & nMesh2::WriteOnce ) usageString += "WriteOnce|";
        else if( usageFlags & nMesh2::ReadOnly ) usageString += "ReadOnly|";
        else if( usageFlags & nMesh2::WriteOnly ) usageString += "WriteOnly|";
        else if( usageFlags & nMesh2::NPatch ) usageString += "NPatch|";
        else if( usageFlags & nMesh2::RTPatch ) usageString += "RTPatch|";
        else if( usageFlags & nMesh2::PointSprite ) usageString += "PointSprite|";
        else if( usageFlags & nMesh2::NeedsVertexShader ) usageString += "NeedsVertexShader|";
        else n_error( "Invalid usage flags %d in n_getmeshusage (nShapeNode)!", usageFlags );
    }
    return usageString;
}
//------------------------------------------------------------------------------
/**
    Converts a set of usage flags represented as a string of the form
    "ReadOnly|NeedsVertexShader"
    into the corresponding integer representation.
    Trailing '|'s are ignored, as is case.
*/
int nMesh2::ConvertUsageStringToFlags( const char* usageFlagsString )
{
    int usage = 0;
    nString workingString = usageFlagsString;
    if( !workingString.IsEmpty() )
    {
        workingString.ToLower();
        const char* flagString = workingString.GetFirstToken( "|" );
        while (flagString)
        {   
            if( !strcmp( flagString, "writeonce" ) ) usage |= nMesh2::WriteOnce;
            else if( !strcmp( flagString, "readonly" ) ) usage |= nMesh2::ReadOnly;
            else if( !strcmp( flagString, "writeonly" ) ) usage |= nMesh2::WriteOnly;
            else if( !strcmp( flagString, "npatch" ) ) usage |= nMesh2::NPatch;
            else if( !strcmp( flagString, "rtpatch" ) ) usage |= nMesh2::RTPatch;
            else if( !strcmp( flagString, "pointsprite" ) ) usage |= nMesh2::PointSprite;
            else if( !strcmp( flagString, "needsvertexshader" ) ) usage |= nMesh2::NeedsVertexShader;
            else n_error( "Invalid flag string '%s' in n_setmeshusage (nShapeNode)!", flagString );
            flagString = workingString.GetNextToken( "|" );
        }
    }
    return usage;
}

