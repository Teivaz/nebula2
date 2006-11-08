#ifndef GRAPHICS_ENTITY_H
#define GRAPHICS_ENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::Entity

    A graphics entity knows how to render itself.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/server.h"
#include "mathlib/matrix.h"
#include "mathlib/bbox.h"
#include "scene/nrendercontext.h"
#include "graphics/resource.h"
#include "misc/nwatched.h"
#include "scene/ntransformnode.h"
#include "game/time/timesource.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Server;
class Cell;
class Resource;

class Entity : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Entity);

public:
    /// clip status
    enum ClipStatus
    {
        InvalidClipStatus,  ///< visibility status invalid
        Inside,             ///< cell is inside clip volume
        Outside,            ///< cell is outside clip volume
        Clipped,            ///< cell is partly in clip volume
    };

    /// entity types
    enum Type
    {
        Shape = 0,      // a shape entity
        Light,          // a light entity
        Camera,         // a camera entity

        NumTypes,
    };

    /// link types
    enum LinkType
    {
        CameraLink = 0,     // a camera link
        LightLink,          // a light link
        PickupLink,         // a pickup link

        NumLinkTypes,
    };

    /// constructor
    Entity();
    /// destructor
    virtual ~Entity();
    /// get entity type
    virtual Entity::Type GetType() const;
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// called when attached to graphics cell
    virtual void OnAttachedToCell(Cell* cell);
    /// called when removed from graphics cell
    virtual void OnRemovedFromCell();
    /// get clip status against bounding box
    virtual ClipStatus GetBoxClipStatus(const bbox3& box);
    /// called before rendering happens
    virtual void OnRenderBefore();
    /// called after rendering has happened
    virtual void OnRenderAfter();
    /// render the graphics entity
    virtual void Render();
    /// set the current world space transformation
    virtual void SetTransform(const matrix44& m);
    /// get the current world space transformation
    const matrix44& GetTransform() const;

    /// set an optional external time source, otherwise use Graphics::Server's time
    void SetTimeSource(Game::TimeSource* timeSource);
    /// get optional time source, can be 0
    Game::TimeSource* GetTimeSource() const;
    /// set time factor
    void SetTimeFactor(float f);
    /// get time factor
    float GetTimeFactor() const;
    /// reset the activation time stamp
    void ResetActivationTime();
    /// get entity's activation time
    nTime GetActivationTime() const;
    /// get current entity local time
    nTime GetEntityTime() const;
    /// set entity visibility status
    void SetVisible(bool b);
    /// get entity visibility status
    bool GetVisible() const;
    /// set max visible distance
    void SetMaxDistance(float d);
    /// get max visible distance
    float GetMaxDistance() const;
    /// set min visible screen size
    void SetMinSize(float s);
    /// get min visible screen size
    float GetMinSize() const;
    /// check if visible by distance and screen size
    bool TestLodVisibility();

    /// set name of graphics resource
    void SetResourceName(const nString& n);
    /// get name of graphics resource
    const nString& GetResourceName() const;
    /// get embedded resource object
    const Resource& GetResource() const;
    /// set optional shadow resource name
    void SetShadowResourceName(const nString& n);
    /// get optional shadow resource name
    const nString& GetShadowResourceName() const;
    /// get shadow resource object
    const Resource& GetShadowResource() const;

    /// modify the local bounding box, usually this is computed automatically on creation
    void SetLocalBox(const bbox3& box);
    /// get the entity's local bounding box in model space
    const bbox3& GetLocalBox() const;
    /// get the computed bounding box in global space
    const bbox3& GetBox();
    /// get the bounding box extended to take shadows from linked lights into account
    const bbox3& GetShadowBox();

    /// get the cell the entity is currently attached to
    Cell* GetCell() const;
    /// clear links (decrease refcount)
    void ClearLinks(LinkType linkType);
    /// add a link (increase refcount)
    void AddLink(LinkType linkType, Entity* entity);
    /// get number of links
    int GetNumLinks(LinkType linkType) const;
    /// get link at index
    Entity* GetLinkAt(LinkType linkType, int index) const;
    /// get Nebula2 render context of the entity
    nRenderContext& GetRenderContext();
    /// render a debug visualization of the entity
    virtual void RenderDebug();
    /// set a Nebula2 render flag
    void SetRenderFlag(nRenderContext::Flag f, bool b);
    /// get a Nebula2 render flag
    bool GetRenderFlag(nRenderContext::Flag f) const;
    /// set user data, this is a dirty way to get this graphics entity's game entity
    void SetUserData(int d);
    /// get user data
    int GetUserData() const;

protected:
    /// update the render context variables
    virtual void UpdateRenderContextVariables();
    /// set the cell the entity is currently attached to
    void SetCell(Cell* cell);
    /// update the global space bounding box
    virtual void UpdateGlobalBox();
    /// update the entity's position inside the cell tree
    void UpdatePositionInCellTree();
    /// make sure our resource is loaded
    void ValidateResource();

    bool active;                                // between OnActivate()/OnDeactivate()
    bool visible;
    bool globalBoxDirty;

    Cell* cell;                                 // currently attached to this cell
    Resource resource;                          // the graphics resource object
    Resource shadowResource;                    // the optional shadow resource object
    bbox3 localBox;                             // the object's local bounding box
    bbox3 globalBox;                            // the object's bounding box in global space
    bbox3 shadowBox;                            // global box extruded to take shadows into account

    uint shadowBoxUpdatedFrameId;               // ...so that shadow box is only computed once per frame

    nVariable::Handle timeVarHandle;            // variable handle for current time
    nVariable::Handle oneVarHandle;             // constant 1.0 channel
    nVariable::Handle windVarHandle;            // the wind channel

    nRenderContext renderContext;               // Nebula render context
    nRenderContext shadowRenderContext;         // optional shadow render context
    matrix44 transform;
    nFixedArray<nArray<Ptr<Entity> > > linkArray;   // current links

    nTime activateTime;
    float timeFactor;
    Ptr<Game::TimeSource> extTimeSource;
    float maxVisibleDistance;
    float minVisibleSize;

    int userData;
};

RegisterFactory(Entity);

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetMinSize(float s)
{
    this->minVisibleSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Entity::GetMinSize() const
{
    return this->minVisibleSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetMaxDistance(float d)
{
    this->maxVisibleDistance = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Entity::GetMaxDistance() const
{
    return this->maxVisibleDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetTimeSource(Game::TimeSource* timeSource)
{
    this->extTimeSource = timeSource;
}

//------------------------------------------------------------------------------
/**
*/
inline
Game::TimeSource*
Entity::GetTimeSource() const
{
    return this->extTimeSource.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetTimeFactor(float f)
{
    this->timeFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Entity::GetTimeFactor() const
{
    return this->timeFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Entity::GetActivationTime() const
{
    return this->activateTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderContext&
Entity::GetRenderContext()
{
    return this->renderContext;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Resource&
Entity::GetResource() const
{
    return this->resource;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Resource&
Entity::GetShadowResource() const
{
    return this->shadowResource;
}

//------------------------------------------------------------------------------
/**
    @return     bounding box in global space
*/
inline
const bbox3&
Entity::GetBox()
{
    if (this->globalBoxDirty)
    {
        this->UpdateGlobalBox();
    }
    return this->globalBox;
}

//------------------------------------------------------------------------------
/**
    @return     bounding box in model space
*/
inline
const bbox3&
Entity::GetLocalBox() const
{
    return this->localBox;
}

//------------------------------------------------------------------------------
/**
    Get the current transformation in world space.

    @return     the current world space transform
*/
inline
const matrix44&
Entity::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetLocalBox(const bbox3& box)
{
    this->localBox = box;
    this->globalBoxDirty = true;
}

//------------------------------------------------------------------------------
/**
    Update the internal global bounding box.
*/
inline
void
Entity::UpdateGlobalBox()
{
    n_assert(this->globalBoxDirty);
    this->globalBoxDirty = false;
    this->globalBox = this->localBox;
    if (this->resource.GetName().IsValid())
    {
        this->globalBox.transform(this->resource.GetNode()->GetTransform() * this->GetTransform());
    }
    else
    {
        this->globalBox.transform(this->GetTransform());
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Entity::SetVisible(bool b)
{
    this->visible = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Entity::GetVisible() const
{
    return this->visible;
}

//------------------------------------------------------------------------------
/**
    Add an entity link. This will increment the refcount of the linked
    entity.

    @param  linkType    the link type
    @param  entity      pointer to a graphics entity object
*/
inline
void
Entity::AddLink(LinkType linkType, Entity* entity)
{
    n_assert(entity);
    this->linkArray[linkType].Append(entity);
}

//------------------------------------------------------------------------------
/**
    Set a render hint. This goes directly into the render contexts.
*/
inline
void
Entity::SetRenderFlag(nRenderContext::Flag f, bool b)
{
    this->renderContext.SetFlag(f, b);
    if (this->shadowRenderContext.IsValid())
    {
        this->shadowRenderContext.SetFlag(f, b);
    }
}

//------------------------------------------------------------------------------
/**
    Return a render hint.
*/
inline
bool
Entity::GetRenderFlag(nRenderContext::Flag f) const
{
    return this->renderContext.GetFlag(f);
}

//------------------------------------------------------------------------------
/**
    Set the user data of the entity. This is a not-quite-so-nice way to
    store the game entity's unique id in the physics entity.
*/
inline
void
Entity::SetUserData(int d)
{
    this->userData = d;
}

//------------------------------------------------------------------------------
/**
    Get the user data associated with this physics entity.
*/
inline
int
Entity::GetUserData() const
{
    return this->userData;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
