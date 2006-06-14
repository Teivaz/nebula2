#ifndef GRAPHICS_SERVER_H
#define GRAPHICS_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Server

    The Graphics::Server is the central object of the graphics subsystem.

    Graphics Subsystem Overview:

    The tasks of the graphics subsystem are:

    - display initialization and management
    - optimal rendering of graphics entities
    - culling

    The lowlevel task of rendering and resource management is left to Nebula.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nref.h"
#include "util/narray.h"
#include "foundation/server.h"
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "misc/nwatched.h"
#include "graphics/level.h"
#include "gfx2/ndisplaymode2.h"
#include "graphics/entity.h"
#include "graphics/animtable.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Cell;
class Resource;
class Level;

class Server : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get server pointer
    static Server* Instance();
    /// set the display mode (outside of Open()/Close())
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get the initial display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional render path
    void SetRenderPath(const nString& s);
    /// get optional render path
    const nString& GetRenderPath() const;
    /// set optional feature set
    void SetFeatureSet(const nString& s);
    /// get optional feature set
    const nString& GetFeatureSet() const;
    /// initialize the graphics subsystem
    bool Open();
    /// close the graphics subsystem
    void Close();
    /// trigger the graphics subsystem (does not render, but runs the window system's message pump)
    bool Trigger();
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// set current frame time
    void SetFrameTime(nTime t);
    /// get current frame time
    nTime GetFrameTime() const;
    /// begin rendering the scene
    bool BeginRender();
    /// render the current frame of the current level object
    void Render();
    /// render debug visualizations
    void RenderDebug();
    /// finish rendering the scene
    void EndRender();
    /// set the current graphics level object (incrs refcount)
    void SetLevel(Level* level);
    /// get the current graphics level object
    Level* GetLevel() const;
    /// get the current camera
    CameraEntity* GetCamera() const;
    /// get current frame id
    uint GetFrameId() const;
    /// drag drop select
    void DragDropSelect(const vector3& lookAt, float angleOfView, float aspectRatio, nArray<Ptr<Entity> >& entities);

private:
    friend class LightEntity;
    friend class Entity;
    friend class Cell;

    #if __NEBULA_STATS__
    /// statistics: num visible entities from light or camers
    void AddNumVisibleEntities(Entity::Type observedType, Entity::LinkType linkType, int incr);
    /// statistics: num cells visited for camera visibility checks
    void AddNumVisitedCells(Entity::LinkType linkType, int incr);
    /// statistics: num cells classified as outside
    void AddNumOutsideCells(Entity::LinkType linkType, int incr);
    /// statistics: num cells classified as inside
    void AddNumVisibleCells(Entity::LinkType linkType, int incr);
    #endif

    static Server* Singleton;

    nDisplayMode2 displayMode;
    bool isOpen;
    Ptr<Level> curLevel;
    Ptr<AnimTable> animTable;

    uint frameId;                       // a unique frameId
    nString renderPath;
    nString featureSet;
    nTime time;
    nTime frameTime;

    #if __NEBULA_STATS__
    nWatched numShapesVisible;
    nWatched numLightsVisible;
    nWatched numShapesLit;
    nWatched numCellsVisitedCamera;
    nWatched numCellsVisitedLight;
    nWatched numCellsOutsideCamera;
    nWatched numCellsOutsideLight;
    nWatched numCellsVisibleCamera;
    nWatched numCellsVisibleLight;
    nProfiler profMangaRender_GfxRender;
    nProfiler profMangaRender_GfxEndRender_LevelEndRender;
    nProfiler profMangaRender_GfxEndRender_CaptureTrigger;
    nProfiler profMangaRender_GfxEndRender_PresentScene;
    #endif

};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetRenderPath(const nString& s)
{
    this->renderPath = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetFeatureSet(const nString& s)
{
    this->featureSet = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetFeatureSet() const
{
    return this->featureSet;
}

//------------------------------------------------------------------------------
/**
    Set the initial display mode. Will take effect on the next call
    to Open().
*/
inline
void
Server::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
Server::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
CameraEntity*
Server::GetCamera() const
{
    if (this->curLevel != 0)
    {
        return this->curLevel->GetCamera();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Server::GetFrameId() const
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetTime(nTime t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Server::SetFrameTime(nTime t)
{
    this->frameTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
#if __NEBULA_STATS__
inline
void
Server::AddNumVisibleEntities(Entity::Type observedType, Entity::LinkType linkType, int incr)
{
    if (observedType == Entity::Light)
    {
        this->numLightsVisible->SetI(this->numLightsVisible->GetI() + incr);
        return;
    }
    else
    {
        if (linkType == Entity::LightLink)
        {
            this->numShapesLit->SetI(this->numShapesLit->GetI() + incr);
            return;
        }
        else
        {
            this->numShapesVisible->SetI(this->numShapesVisible->GetI() + incr);
            return;
        }
    }
    // can't happen
    n_assert(false);
}
#endif

//------------------------------------------------------------------------------
/**
*/
#if __NEBULA_STATS__
inline
void
Server::AddNumVisitedCells(Entity::LinkType linkType, int incr)
{
    if (linkType == Entity::LightLink)
    {
        this->numCellsVisitedLight->SetI(this->numCellsVisitedLight->GetI() + incr);
    }
    else
    {
        this->numCellsVisitedCamera->SetI(this->numCellsVisitedCamera->GetI() + incr);
    }
}
#endif

//------------------------------------------------------------------------------
/**
*/
#if __NEBULA_STATS__
inline
void
Server::AddNumOutsideCells(Entity::LinkType linkType, int incr)
{
    if (linkType == Entity::LightLink)
    {
        this->numCellsOutsideLight->SetI(this->numCellsOutsideLight->GetI() + incr);
    }
    else
    {
        this->numCellsOutsideCamera->SetI(this->numCellsOutsideCamera->GetI() + incr);
    }
}
#endif

//------------------------------------------------------------------------------
/**
*/
#if __NEBULA_STATS__
inline
void
Server::AddNumVisibleCells(Entity::LinkType linkType, int incr)
{
    if (linkType == Entity::LightLink)
    {
        this->numCellsVisibleLight->SetI(this->numCellsVisibleLight->GetI() + incr);
    }
    else
    {
        this->numCellsVisibleCamera->SetI(this->numCellsVisibleCamera->GetI() + incr);
    }
}
#endif

}; // namespace Graphics
//------------------------------------------------------------------------------
#endif
