//------------------------------------------------------------------------------
//  nshapenode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nshapenode.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nShapeNode, "nmaterialnode");

//------------------------------------------------------------------------------
/**
*/
nShapeNode::nShapeNode() :
    groupIndex(0),
    meshUsage(nMesh2::WriteOnce)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShapeNode::~nShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should call SetMeshUsage in their
    constructors to ensure that this method reflects their requirements.

    @return     a combination on nMesh2::Usage flags
*/
int
nShapeNode::GetMeshUsage() const
{
    return meshUsage;
}
 
//------------------------------------------------------------------------------
/**
    Specifies the mesh usage flag combination required by
    this shape node class.
*/
void
nShapeNode::SetMeshUsage(int usage)
{
    meshUsage = usage;
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShapeNode::UnloadMesh()
{
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Load new mesh, release old one if valid. Also initializes the groupIndex
    member.
*/
bool
nShapeNode::LoadMesh()
{
    if ((!this->refMesh.isvalid()) && (!this->meshName.IsEmpty()))
    {
        // append mesh usage to mesh resource name
        nString resourceName = this->meshName.Get();
        resourceName.Append("_"); 
        resourceName.AppendInt(this->GetMeshUsage());

        // get a new or shared mesh
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(resourceName.Get());
        n_assert(mesh);
        if (!mesh->IsValid())
        {
            mesh->SetFilename(this->meshName.Get());
            mesh->SetUsage(this->GetMeshUsage());

            if (refMeshResourceLoader.isvalid())
            {
                mesh->SetResourceLoader(refMeshResourceLoader.getname());
            }

            if (!mesh->Load())
            {
                n_printf("nMeshNode: Error loading mesh '%s'\n", this->meshName.Get());
                mesh->Release();
                return false;
            }
        }
        this->refMesh = mesh;
        this->SetLocalBox(this->refMesh->GetGroup(this->groupIndex).GetBoundingBox());
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nShapeNode::LoadResources()
{
    if (nMaterialNode::LoadResources())
    {
        this->LoadMesh();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nShapeNode::UnloadResources()
{
    nMaterialNode::UnloadResources();
    this->UnloadMesh();
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide geometry
*/
bool
nShapeNode::HasGeometry() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Perform pre-instancing actions needed for rendering geometry. This
    is called once before multiple instances of this shape node are
    actually rendered.
*/
bool
nShapeNode::ApplyGeometry(nSceneServer* sceneServer)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(this->refMesh->IsValid());

    // set mesh, vertex and index range
    gfxServer->SetMesh(this->refMesh.get());
    const nMeshGroup& curGroup = this->refMesh->GetGroup(this->groupIndex);
    gfxServer->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
    gfxServer->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());
    return true;
}

//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexed().

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    nGfxServer2::Instance()->DrawIndexedNS(nGfxServer2::TriangleList);
    return true;
}

//------------------------------------------------------------------------------
/**
    Set the resource name. The mesh resource name consists of the
    filename of the mesh.
*/
void
nShapeNode::SetMesh(const char* name)
{
    n_assert(name);
    this->UnloadMesh();
    this->meshName = name;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nShapeNode::GetMesh() const
{
    return this->meshName.IsEmpty() ? 0 : this->meshName.Get();
}

//------------------------------------------------------------------------------
/**
    Set the resource loader used to load the mesh data.  If this is NULL, then
    the mesh is loaded through the default mesh loading code.
*/
void
nShapeNode::SetMeshResourceLoader(const char* resourceLoaderPath)
{
    this->refMeshResourceLoader = resourceLoaderPath;
}

//------------------------------------------------------------------------------
/**
    Get the mesh resource loader.
    
    @return resource loader name or null when there is no resource loader
*/
const char *
nShapeNode::GetMeshResourceLoader()
{
    if (this->refMeshResourceLoader.isvalid())
    {
        return this->refMeshResourceLoader.getname();
    }
    else
    {
        return 0;
    }
}

