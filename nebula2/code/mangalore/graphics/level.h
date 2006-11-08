#ifndef GRAPHICS_LEVEL_H
#define GRAPHICS_LEVEL_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Level

    The Level class contains all Cell and Entity
    objects in a level and is responsible for rendering them efficiently.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/server.h"
#include "graphics/cameraentity.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;
class Entity;
class LightEntity;
class CameraEntity;

class Level : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Level);

public:
    /// constructor
    Level();
    /// destructor
    virtual ~Level();
    /// set the root cell of the level, bumps refcount of cell
    void SetRootCell(Cell* cell);
    /// get the root cell of the level
    Cell* GetRootCell() const;
    /// attach a dynamic graphics entity to the level
    void AttachEntity(Entity* entity);
    /// remove a dynamic graphics entity from the level
    void RemoveEntity(Entity* entity);
    /// set the current camera entity (increase refcount)
    void SetCamera(CameraEntity* camera);
    /// get the current camera entity
    CameraEntity* GetCamera() const;
    /// prepare for rendering, return false if it fails
    bool BeginRender();
    /// render the current frame of the level
    void Render();
    /// render debug visualization of the level object
    void RenderDebug();
    /// finish rendering
    void EndRender();

private:
    Ptr<Cell> rootCell;
    Ptr<CameraEntity> defaultCamera;
    Ptr<CameraEntity> curCamera;

    PROFILER_DECLARE(profFindVisibleLights);
    PROFILER_DECLARE(profFindLitObjects);
    PROFILER_DECLARE(profFindVisibleObjects);
    PROFILER_DECLARE(profCameraRenderBefore);
    PROFILER_DECLARE(profCameraRender);
    PROFILER_DECLARE(profClearLinks);
};

RegisterFactory(Level);

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
