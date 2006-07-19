//------------------------------------------------------------------------------
//  graphics/level.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/level.h"
#include "graphics/cell.h"
#include "gfx2/ngfxserver2.h"
#include "particle/nparticleserver.h"
#include "scene/nsceneserver.h"
#include "graphics/cameraentity.h"
#include "graphics/lightentity.h"
#include "misc/nconserver.h"
#include "graphics/server.h"

namespace Graphics
{
ImplementRtti(Graphics::Level, Foundation::RefCounted);
ImplementFactory(Graphics::Level);

//------------------------------------------------------------------------------
/**
*/
Level::Level()
{
    this->defaultCamera.create();

    PROFILER_INIT(this->profFindVisibleLights, "profMangaGfxFindVisibleLights");
    PROFILER_INIT(this->profFindLitObjects, "profMangaGfxFindLitObjects");
    PROFILER_INIT(this->profFindVisibleObjects, "profMangaGfxFindVisibleObjects");
    PROFILER_INIT(this->profCameraRenderBefore, "profMangaGfxCameraRenderBefore");
    PROFILER_INIT(this->profCameraRender, "profMangaGfxCameraRender");
    PROFILER_INIT(this->profClearLinks, "profMangaGfxClearLinks");
}

//------------------------------------------------------------------------------
/**
*/
Level::~Level()
{
    this->SetRootCell(0);
}

//------------------------------------------------------------------------------
/**
    Set current root cell. This is the top most graphics cell which contains
    the graphics objects for the entire level. The reference count of
    the root cell is incremented. If a previous cell object is attached
    its refcount will be decremented. A 0 cell pointer is allowed, this will
    just release the previous root cell object.

    @param  cell    pointer to Cell object
*/
void
Level::SetRootCell(Cell* cell)
{
    if (this->rootCell != 0)
    {
        this->SetCamera(0);
        this->rootCell->OnRemoveFromLevel();
    }
    this->rootCell = cell;
    if (rootCell != 0)
    {
        this->rootCell->OnAttachToLevel(this);
        this->SetCamera(this->defaultCamera);
    }
}

//------------------------------------------------------------------------------
/**
    Get the current root cell of the level, this will not change the refcount
    of the object.

    @return     pointer to current root cell (can be 0)
*/
Cell*
Level::GetRootCell() const
{
    return this->rootCell;
}

//------------------------------------------------------------------------------
/**
    Set the current camera entity. This is the camera the player looks through.
    The camera entity must be currently attached to this level. The ref count
    of the previous camera will be decremented, and the refcount of the
    new camera object incremented.

    @param  newCamera  pointer to a camera entity (a 0 pointer is valid)
*/
void
Level::SetCamera(CameraEntity* newCamera)
{
    if (curCamera != 0)
    {
        this->RemoveEntity(this->curCamera);
    }
    this->curCamera = newCamera;
    if (newCamera)
    {
        this->AttachEntity(this->curCamera);
    }
}

//------------------------------------------------------------------------------
/**
    Get the current camera.

    @return     pointer to a camera entity (can be 0)
*/
CameraEntity*
Level::GetCamera() const
{
    return this->curCamera.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Attach a dynamic graphics entity. The graphics entity will be sorted
    correctly into the cell tree. The refcount of the entity will be
    incremented.

    @param  entity  pointer to a graphics entity
*/
void
Level::AttachEntity(Entity* entity)
{
    n_assert(this->rootCell != 0);
    entity->SetMaxDistance(Server::Instance()->GetDistThreshold(entity->GetRtti()));
    entity->SetMinSize(Server::Instance()->GetSizeThreshold(entity->GetRtti()));
    entity->OnActivate();
    this->rootCell->InsertEntity(entity);
}

//------------------------------------------------------------------------------
/**
    Remove a dynamic graphics entity from the level.

    @param  entity  pointer to the graphics entity to remove
*/
void
Level::RemoveEntity(Entity* entity)
{
    Cell* cell = entity->GetCell();
    if (cell)
    {
        cell->RemoveEntity(entity);
    }
    entity->OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
bool
Level::BeginRender()
{
    n_assert(this->rootCell != 0);
    n_assert(this->curCamera != 0);

    Foundation::Server* fndServer = Foundation::Server::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    // clear light and camera links
    PROFILER_START(this->profClearLinks);
    this->rootCell->ClearLinks(Entity::CameraLink);
    this->rootCell->ClearLinks(Entity::LightLink);
    PROFILER_STOP(this->profClearLinks);

    // compute the current view/projection matrix
    const matrix44& view = this->curCamera->GetTransform();
    const matrix44& proj = this->curCamera->GetCamera().GetProjection();
    matrix44 viewProjection = view * proj;

    // first, get all visible light sources
    // NOTE: at first glance it look bad to traverse twice through
    // the tree for camera visibility (first for lights, then for
    // shapes). Unfortunately this is necessary because entities
    // need to compute their shadow bounding boxes for view culling,
    // so they need their light links updated first!
    PROFILER_START(this->profFindVisibleLights);
    this->rootCell->UpdateLinks(this->curCamera, Entity::Light, Entity::CameraLink);
    PROFILER_STOP(this->profFindVisibleLights);

    // for each light source, gather entities lit by this light source
    PROFILER_START(this->profFindLitObjects);
    int numVisibleLights = this->curCamera->GetNumLinks(Entity::CameraLink);
    int visibleLightIndex;
    for (visibleLightIndex = 0; visibleLightIndex < numVisibleLights; visibleLightIndex++)
    {
        Entity* lightEntity = this->curCamera->GetLinkAt(Entity::CameraLink, visibleLightIndex);
        n_assert(lightEntity->GetType() == Entity::Light);
        this->rootCell->UpdateLinks(lightEntity, Entity::Shape, Entity::LightLink);
    }
    PROFILER_STOP(this->profFindLitObjects);

    // gather shapes visible from the camera
    PROFILER_START(this->profFindVisibleObjects);
    this->rootCell->UpdateLinks(this->curCamera, Entity::Shape, Entity::CameraLink);
    PROFILER_STOP(this->profFindVisibleObjects);

    // begin rendering the scene
    if (sceneServer->BeginScene(this->curCamera->GetTransform()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Render the current frame of the level.
*/
void
Level::Render()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    PROFILER_START(this->profCameraRenderBefore);
    this->curCamera->OnRenderBefore();
    PROFILER_STOP(this->profCameraRenderBefore);

    PROFILER_START(this->profCameraRender);
    this->curCamera->Render();
    PROFILER_STOP(this->profCameraRender);

    sceneServer->EndScene();
    sceneServer->RenderScene();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the current level.
*/
void
Level::RenderDebug()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginShapes();
    this->rootCell->RenderDebug();
    gfxServer->EndShapes();
}

//------------------------------------------------------------------------------
/**
    Finish rendering.
*/
void
Level::EndRender()
{
    // empty
}

} // namespace Graphics
