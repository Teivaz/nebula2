//------------------------------------------------------------------------------
//  nterrainnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ncterrain2/nterrainnode.h"
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "misc/nwatched.h"

nNebulaScriptClass(nTerrainNode, "nmaterialnode");

//------------------------------------------------------------------------------
/**
*/
nTerrainNode::nTerrainNode() :
    refResourceServer("/sys/servers/resource"),
    maxPixelError(5.0f),
    maxTexelSize(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTerrainNode::~nTerrainNode()
{
    if (this->AreResourcesValid())
    {
        this->UnloadResources();
    }
}

//------------------------------------------------------------------------------
/**
    Load resources needed by this object.
*/
bool
nTerrainNode::LoadResources()
{
    if (nMaterialNode::LoadResources())
    {
        if ((!this->refChunkLodTree.isvalid()) && (!this->chunkFilename.IsEmpty()))
        {
            nChunkLodTree* tree = (nChunkLodTree*) this->refResourceServer->NewResource("nchunklodtree", 0, nResource::Other);
            n_assert(tree);
            if (!tree->IsValid())
            {
                tree->SetFilename(this->chunkFilename);
                tree->SetTqtFilename(this->texQuadFile.Get());
                tree->SetDisplayMode(nGfxServer2::Instance()->GetDisplayMode());
                tree->SetCamera(nGfxServer2::Instance()->GetCamera());
                tree->SetMaxPixelError(this->maxPixelError);
                tree->SetMaxTexelSize(this->maxTexelSize);
                tree->SetTerrainScale(this->terrainScale);
                tree->SetTerrainOrigin(this->terrainOrigin);
                if (!tree->Load())
                {
                    n_printf("nTerrainNode: Error loading .chu file %s\n", this->chunkFilename.Get());
                    tree->Release();
                    return false;
                }
            }
            this->refChunkLodTree = tree;
            this->SetLocalBox(tree->GetBoundingBox());
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload resources.
*/
void
nTerrainNode::UnloadResources()
{
    nMaterialNode::UnloadResources();
    if (this->refChunkLodTree.isvalid())
    {
        this->refChunkLodTree->Release();
        this->refChunkLodTree.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Indicate to scene server that we provide geometry
*/
bool
nTerrainNode::HasGeometry() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexed().

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nTerrainNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);

    nWatched watchNumMeshesRendered("terrainNumMeshesRendered", nArg::Int);
    nWatched watchNumTexturesRendered("terrainNumTexturesRendered", nArg::Int);
    nWatched watchNumMeshesAllocated("terrainNumMeshesAllocated", nArg::Int);
    nWatched watchNumTexturesAllocated("terrainNumTexturesAllocated", nArg::Int);

    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // only render if resource is available (may not be 
    // available yet if async resource loading enabled)
    if (!this->refChunkLodTree->IsValid())
    {
        return false;
    }

    // update and render the chunk lod tree
    const matrix44& view = gfxServer->GetTransform(nGfxServer2::InvView);
    this->refChunkLodTree->Update(view.pos_component());

    // render in 2 passes, far and near because of limited z buffer precision
    const float farZ = 160000.0f;
    nCamera2 camera = gfxServer->GetCamera();
    nCamera2 origCamera = camera;

    // render far pass and near pass with different near/far planes to
    // workaround zbuffer precision problems
    nChunkLodTree* chunkLodTree = this->refChunkLodTree.get();
    chunkLodTree->BeginRender();
    camera.SetNearPlane(origCamera.GetFarPlane() * 0.9f);
    camera.SetFarPlane(farZ);
    gfxServer->SetCamera(camera);
    chunkLodTree->Render(gfxServer->GetShader(), gfxServer->GetTransform(nGfxServer2::Projection), gfxServer->GetTransform(nGfxServer2::ModelViewProjection));

    gfxServer->Clear(nGfxServer2::DepthBuffer, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0);
    camera.SetNearPlane(origCamera.GetNearPlane());
    camera.SetFarPlane(origCamera.GetFarPlane());
    gfxServer->SetCamera(camera);
    chunkLodTree->Render(gfxServer->GetShader(), gfxServer->GetTransform(nGfxServer2::Projection), gfxServer->GetTransform(nGfxServer2::ModelViewProjection));
    chunkLodTree->EndRender();

    watchNumMeshesRendered->SetI(chunkLodTree->GetNumMeshesRendered());
    watchNumTexturesRendered->SetI(chunkLodTree->GetNumTexturesRendered());
    watchNumMeshesAllocated->SetI(chunkLodTree->GetNumMeshesAllocated());
    watchNumTexturesAllocated->SetI(chunkLodTree->GetNumTexturesAllocated());

    gfxServer->SetCamera(origCamera);
    return true;
}
