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
    Returns mesh2 object. If not loaded yet, it will be loaded.
*/
nMesh2*
nShapeNode::GetMeshObject()
{
    if (!this->refMesh.isvalid())
    {
        n_assert(this->LoadMesh());
    }
    return this->refMesh.get();
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
        nString resourceName;
        resourceName.Format("%s_%d", this->meshName.Get(), this->GetMeshUsage());

        // get a new or shared mesh
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(resourceName);
        n_assert(mesh);
        if (!mesh->IsLoaded())
        {
            mesh->SetFilename(this->meshName);
            mesh->SetUsage(this->GetMeshUsage());

            if (this->refMeshResourceLoader.isvalid())
            {
                mesh->SetResourceLoader(this->refMeshResourceLoader.getname());
            }

            if (!mesh->Load())
            {
                n_printf("nMeshNode: Error loading mesh '%s'\n", this->meshName.Get());
                mesh->Release();
                return false;
            }
        }
        this->refMesh = mesh;
        this->SetLocalBox(this->refMesh->Group(this->groupIndex).GetBoundingBox());
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
nShapeNode::ApplyGeometry(nSceneServer* /*sceneServer*/)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(this->refMesh->IsValid());

    // set mesh, vertex and index range
    gfxServer->SetMesh(this->refMesh, this->refMesh);
    const nMeshGroup& curGroup = this->refMesh->Group(this->groupIndex);
    gfxServer->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
    gfxServer->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());
    return true;
}

//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexedNS().

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nShapeNode::RenderGeometry(nSceneServer* /*sceneServer*/, nRenderContext* /*renderContext*/)
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
nShapeNode::SetMesh(const nString& name)
{
    n_assert(name.IsValid());
    this->UnloadMesh();
    this->meshName = name;
}

//------------------------------------------------------------------------------
/**
*/
const nString&
nShapeNode::GetMesh() const
{
    return this->meshName;
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

//------------------------------------------------------------------------------
/**
    Render debugging information for this mesh (normals, tangents, binormals).
*/
void
nShapeNode::RenderDebug(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& modelMatrix)
{
/*
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    const float length = 0.025f;
    const nMeshGroup& curGroup = this->refMesh->Group(this->groupIndex);
    int firstVertex = curGroup.GetFirstVertex();
    int numVertices = curGroup.GetNumVertices();
    int vWidth = this->refMesh->GetVertexWidth();

    float* vBuffer = this->refMesh->LockVertices() + firstVertex * vWidth;
    gfxServer->BeginShapes();

    // draw normals
    if (this->refMesh->HasAllVertexComponents(nMesh2::Normal))
    {
        nFixedArray<vector3> lines(numVertices * 2);
        const vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
        vector3 v0, v1;
        int vOffset = this->refMesh->GetVertexComponentOffset(nMesh2::Normal);
        int vIndex;
        for (vIndex = 0; vIndex < numVertices; vIndex++)
        {
            float* vertex = vBuffer + vIndex * vWidth;
            v0.set(vertex[0], vertex[1], vertex[2]);
            v1.set(vertex[vOffset + 0], vertex[vOffset + 1], vertex[vOffset + 2]);
            v1 = v0 + v1 * length;
            lines[vIndex * 2 + 0] = v0;
            lines[vIndex * 2 + 1] = v1;
        }
        gfxServer->DrawShapePrimitives(nGfxServer2::LineList, numVertices, &(lines[0]), 3, modelMatrix, color);
    }

    // draw tangents
    if (this->refMesh->HasAllVertexComponents(nMesh2::Tangent))
    {
        nFixedArray<vector3> lines(numVertices * 2);
        const vector4 color(1.0f, 1.0f, 0.0f, 1.0f);
        vector3 v0, v1;
        int vOffset = this->refMesh->GetVertexComponentOffset(nMesh2::Tangent);
        int vIndex;
        for (vIndex = 0; vIndex < numVertices; vIndex++)
        {
            float* vertex = vBuffer + vIndex * vWidth;
            v0.set(vertex[0], vertex[1], vertex[2]);
            v1.set(vertex[vOffset + 0], vertex[vOffset + 1], vertex[vOffset + 2]);
            v1 = v0 + v1 * length;
            lines[vIndex * 2 + 0] = v0;
            lines[vIndex * 2 + 1] = v1;
        }
        gfxServer->DrawShapePrimitives(nGfxServer2::LineList, numVertices, &(lines[0]), 3, modelMatrix, color);
    }

    // draw binormals
    if (this->refMesh->HasAllVertexComponents(nMesh2::Binormal))
    {
        nFixedArray<vector3> lines(numVertices * 2);
        const vector4 color(0.0f, 0.0f, 1.0f, 1.0f);
        vector3 v0, v1;
        int vOffset = this->refMesh->GetVertexComponentOffset(nMesh2::Binormal);
        int vIndex;
        for (vIndex = 0; vIndex < numVertices; vIndex++)
        {
            float* vertex = vBuffer + vIndex * vWidth;
            v0.set(vertex[0], vertex[1], vertex[2]);
            v1.set(vertex[vOffset + 0], vertex[vOffset + 1], vertex[vOffset + 2]);
            v1 = v0 + v1 * length;
            lines[vIndex * 2 + 0] = v0;
            lines[vIndex * 2 + 1] = v1;
        }
        gfxServer->DrawShapePrimitives(nGfxServer2::LineList, numVertices, &(lines[0]), 3, modelMatrix, color);
    }

    gfxServer->EndShapes();
    this->refMesh->UnlockVertices();
*/
}

