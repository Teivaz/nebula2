#define N_IMPLEMENTS nShapeNode
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
    meshUsage(nMesh2::WriteOnce),
    renderWireframe(false)
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
        nMesh2* mesh = this->refGfxServer->NewMesh(this->meshName.Get());
        n_assert(mesh);
        if (!mesh->IsValid())
        {
            mesh->SetFilename(this->meshName.Get());
            mesh->SetUsage(this->meshUsage);
            if (!mesh->Load())
            {
                n_printf("nMeshNode: Error loading mesh '%s'\n", this->meshName.Get());
                mesh->Release();
                return false;
            }
        }
        this->refMesh = mesh;
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
    Check if resources are valid.
*/
bool
nShapeNode::AreResourcesValid() const
{
    if (nMaterialNode::AreResourcesValid())
    {
        return this->refMesh.isvalid();
    }
    return false;
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
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::Draw().
*/
void
nShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
    nGfxServer2* gfx = this->refGfxServer.get();

    // TODO call geometry manipulators!

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    if (this->renderWireframe)
    {
        // render the mesh in wireframe mode
        this->refMesh->RenderWireframe(gfx, this->refVariableServer.get(), "shaders:wireframe.fx", this->groupIndex);
    }
    else
    {
        // render the mesh in normal mode (always at stream 0)
        gfx->SetMesh(0, this->refMesh.get());

        // set the vertex and index range
        const nMeshGroup& curGroup = this->refMesh->GetGroup(this->groupIndex);
        gfx->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
        gfx->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());
        gfx->Draw();
    }
}

//------------------------------------------------------------------------------
/**
    Set the resource name. The mesh resource name consists of the
    filename of the mesh, and optionally a mesh group name, delimited by '|'.
    If no group name exists, the first group in the mesh file will be used.
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
