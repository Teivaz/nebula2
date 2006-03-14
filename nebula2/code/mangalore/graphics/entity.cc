//------------------------------------------------------------------------------
//  graphics/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/entity.h"
#include "graphics/cell.h"
#include "graphics/resource.h"
#include "variable/nvariableserver.h"
#include "scene/nsceneserver.h"
#include "scene/ntransformnode.h"
#include "graphics/server.h"
#include "gfx2/ngfxserver2.h"
#include "graphics/lightentity.h"

namespace Graphics
{
ImplementRtti(Graphics::Entity, Foundation::RefCounted);
ImplementFactory(Graphics::Entity);

//------------------------------------------------------------------------------
/**
*/
Entity::Entity() :
    active(false),
    cell(0),
    globalBoxDirty(false),
    visible(true),
    linkArray(NumLinkTypes),
    shadowBoxUpdatedFrameId(0),
    activateTime(0.0),
    timeFactor(1.0f)
{
    // initialize link arrays for double grow size,
    // normal entities need rather small arrays, while cameras and
    // lights may need bigger arrays...
    int linkType;
    for (linkType = 0; linkType < NumLinkTypes; linkType++)
    {
        this->linkArray[linkType].SetFlags(nArray<Ptr<Entity> >::DoubleGrowSize);
    }

    // initialize Nebula variable handles
    nVariableServer* varServer = nVariableServer::Instance();
    this->timeVarHandle = varServer->GetVariableHandleByName("time");
    this->oneVarHandle  = varServer->GetVariableHandleByName("one");
    this->windVarHandle = varServer->GetVariableHandleByName("wind");
    
    // initialize render context
    nFloat4 wind = { 1.0f, 0.0f, 0.0f, 0.5f };
    this->renderContext.AddVariable(nVariable(this->timeVarHandle, 0.0f));
    this->renderContext.AddVariable(nVariable(this->oneVarHandle, 1.0f));
    this->renderContext.AddVariable(nVariable(this->windVarHandle, wind));

    this->shadowRenderContext.AddVariable(nVariable(this->timeVarHandle, 0.0f));
    this->shadowRenderContext.AddVariable(nVariable(this->oneVarHandle, 1.0f));
    this->shadowRenderContext.AddVariable(nVariable(this->windVarHandle, wind));
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    n_assert(this->cell == 0);
    if (this->active)
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
Entity::Type
Entity::GetType() const
{
    return Shape;
}

//------------------------------------------------------------------------------
/**
    Validates the embedded resource. Does nothing if everything is ok.
*/
void
Entity::ValidateResource()
{
    if (!this->resource.IsLoaded())
    {
        this->resource.Load();
        nTransformNode* node = this->resource.GetNode();
        n_assert(node);
        this->renderContext.SetRootNode(node);
        node->RenderContextCreated(&(this->renderContext));
        this->SetLocalBox(node->GetLocalBox());

        // initialize optional shadow resource
        if (this->shadowResource.GetName().IsValid())
        {
            this->shadowResource.Load();
            nTransformNode* node = this->shadowResource.GetNode();
            n_assert(node);
            this->shadowRenderContext.SetRootNode(node);
            node->RenderContextCreated(&(this->shadowRenderContext));
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics object becomes active (i.e.
    when it is attached to a game entity.
*/
void
Entity::OnActivate()
{
    n_assert(!this->active);
    n_assert(!this->resource.IsLoaded());
    n_assert(!this->shadowResource.IsLoaded());

    // set the creation time
    this->activateTime = Server::Instance()->GetTime();

    // load the graphics resource, unless this is a camera
    if (this->GetType() != Camera)
    {
        this->ValidateResource();
    }
    this->active = true;
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics object becomes inactive
    (i.e. when it is removed from a game entity)
*/
void
Entity::OnDeactivate()
{
    n_assert(this->active);

    // clear light and camera links
    int linkType;
    for (linkType = 0; linkType < NumLinkTypes; linkType++)
    {
        this->ClearLinks(LinkType(linkType));
    }

    // release graphics resource object
    if (this->resource.IsLoaded())
    {
        nTransformNode* node = this->resource.GetNode();
        n_assert(node);
        node->RenderContextDestroyed(&(this->renderContext));
        this->resource.Unload();
    }
    if (this->shadowResource.IsLoaded())
    {
        nTransformNode* node = this->shadowResource.GetNode();
        n_assert(node);
        node->RenderContextDestroyed(&(this->shadowRenderContext));
        this->shadowResource.Unload();
    }
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called on the entity when it is attached to a
    graphics cell object.

    @param  c       the cell the entity is attached to
*/
void
Entity::OnAttachedToCell(Cell* c)
{
    n_assert(c);
    n_assert(this->cell == 0);
    this->cell = c;
}

//------------------------------------------------------------------------------
/**
    This method is invoked on the entity when it is removed from
    a graphics entity.
*/
void
Entity::OnRemovedFromCell()
{
    n_assert(this->cell != 0);
    this->cell = 0;

    // clear light and camera links
    int linkType;
    for (linkType = 0; linkType < NumLinkTypes; linkType++)
    {
        this->ClearLinks(LinkType(linkType));
    }
}
    
//------------------------------------------------------------------------------
/**
    Set the graphics resource name.

    @param  name    graphics resource name
*/
void
Entity::SetResourceName(const nString& n)
{
    this->resource.SetName(n);
}

//------------------------------------------------------------------------------
/**
    Get the graphics resource name.

    @return     graphics resource name
*/
const nString&
Entity::GetResourceName() const
{
    return this->resource.GetName();
}

//------------------------------------------------------------------------------
/**
    Set the optional shadow resource name. If a shadow resource is defined
    it will be attached to the scene each frame when the graphics entity is
    rendered, additionally to the normal graphics object. A shadow resource
    must be defined if rendering a "deep hierarchy" node, for instance when
    rendering the representation a complex physics object which is made of
    several rigid bodies, where each body defines the position of a 
    visual hierarchy node.

    @param  name    graphics resource name
*/
void
Entity::SetShadowResourceName(const nString& n)
{
    this->shadowResource.SetName(n);
}

//------------------------------------------------------------------------------
/**
    Get the optional shadow resource name.

    @return     graphics resource name
*/
const nString&
Entity::GetShadowResourceName() const
{
    return this->shadowResource.GetName();
}

//------------------------------------------------------------------------------
/**
    Set the current transformation in world space. Should be updated
    every frame.

    @param  m   the new world space transform
*/
void
Entity::SetTransform(const matrix44& m)
{
    this->transform = m;
    this->globalBoxDirty = true;
    this->UpdatePositionInCellTree();
}

//------------------------------------------------------------------------------
/**
    Get clip status of bounding box against this entity. I.e.: Clipped
    means, the boxes intersect. Outside means, the argument box is outside
    my clip space, Inside means, the argument box is inside my clip space.

    @param  box     a bounding box in global space
    @return         resulting clip status
*/
Entity::ClipStatus
Entity::GetBoxClipStatus(const bbox3& box)
{
    bbox3::ClipStatus clipCode = this->GetBox().clipstatus(box);
    switch (clipCode)
    {
        case bbox3::Clipped:    return Clipped;
        case bbox3::Inside:     return Inside;
        default:                return Outside;
    }
}

//------------------------------------------------------------------------------
/**
    Get the current entity-local time.
*/
nTime
Entity::GetEntityTime() const
{
    return this->timeFactor * (Server::Instance()->GetTime() - this->activateTime);
}

//------------------------------------------------------------------------------
/**
    Update the render context variables.
*/
void
Entity::UpdateRenderContextVariables()
{
    float entityTime = (float) this->GetEntityTime();
    this->renderContext.SetTransform(this->transform);
    this->shadowRenderContext.SetTransform(this->transform);
    this->renderContext.GetVariable(this->timeVarHandle)->SetFloat(entityTime);
    if (this->shadowResource.IsLoaded())
    {
        this->renderContext.GetVariable(this->timeVarHandle)->SetFloat(entityTime);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called before the Render() method is invoked on the entity.
*/
void 
Entity::OnRenderBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called after rendering has happened, and the
    scene has been presented.
*/
void
Entity::OnRenderAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Render the graphics entity. This attaches all resource objects of the
    graphics entity to the scene. The method should only be called if the
    graphics object is actually visible!
*/
void
Entity::Render()
{
    n_assert(this->GetVisible());

    // make sure we're valid for rendering
    this->ValidateResource();

    // update render context variables and transformations
    this->UpdateRenderContextVariables();

    // set current frame id
    this->renderContext.SetFrameId(Graphics::Server::Instance()->GetFrameId());

    // update render context light links
    this->renderContext.ClearLinks();
    int numLinks = this->GetNumLinks(LightLink);
    int linkIndex;
    for (linkIndex = 0; linkIndex < numLinks; linkIndex++)
    {
        this->renderContext.AddLink(&(this->GetLinkAt(LightLink, linkIndex)->renderContext));
    }
    if (this->shadowResource.IsLoaded())
    {
        this->shadowRenderContext.ClearLinks();
        for (linkIndex = 0; linkIndex < numLinks; linkIndex++)
        {
            this->shadowRenderContext.AddLink(&(this->GetLinkAt(LightLink, linkIndex)->renderContext));
        }
    }
    
    // update render context's bounding box
    if (this->GetType() == Light)
    {
        // lights use the usual bounding box
        this->renderContext.SetGlobalBox(this->GetBox());
    }
    else
    {
        // shape use the extruded shadow bounding box
        this->renderContext.SetGlobalBox(this->GetShadowBox());
    }

    // attach graphics resource node to scene
    nSceneServer* sceneServer = nSceneServer::Instance();
    sceneServer->Attach(&(this->renderContext));

    // attach optional shadow node
    if (this->shadowResource.IsLoaded())
    {
        this->shadowRenderContext.SetGlobalBox(this->renderContext.GetGlobalBox());
        sceneServer->Attach(&(this->shadowRenderContext));
    }
}

//------------------------------------------------------------------------------
/**
    Clear the links array. This will decrement the refcounts of all linked 
    entities.

    FIXME: hmm, this may be performance problem, since all contained smart
    pointer will calls their destructors. But it's definitely safer that way.

    @param  linkType    the link type    
*/
void
Entity::ClearLinks(LinkType linkType)
{
    this->linkArray[linkType].Clear();
}

//------------------------------------------------------------------------------
/**
    Get number of linked entities.

    @param  linkType    the link type
    @return             number of entities in link array
*/
int
Entity::GetNumLinks(LinkType linkType) const
{
    return this->linkArray[linkType].Size();
}

//------------------------------------------------------------------------------
/**
    Get pointer to a linked entity at index.

    @param  linkType    the link type
    @param  index       an linked entity index
    @return             pointer to entity
*/
Entity*
Entity::GetLinkAt(LinkType linkType, int index) const
{
    n_assert(linkArray[linkType][index] != 0);
    return this->linkArray[linkType][index];
}

//------------------------------------------------------------------------------
/**
    Update the entity's position in the cell tree.
*/
inline
void
Entity::UpdatePositionInCellTree()
{
    // see if we need to move from one cell to another
    if (this->cell != 0)
    {
        Cell* newCell = this->cell->FindEntityContainmentCell(this);
        n_assert(newCell);
        if (this->cell != newCell)
        {
            this->cell->RemoveEntity(this);
            newCell->AttachEntity(this);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set pointer to graphics cell this object currently is attached to.
    If the object is not attached to any cell at the moment, the cell pointer
    must be set to 0.

    @param  c       pointer to a cell object
*/
void
Entity::SetCell(Cell* c)
{
    this->cell = c;
}

//------------------------------------------------------------------------------
/**
    Get the pointer to the graphics Cell this entity is currently 
    attached to. The pointer can be 0 if the object is not attached
    to any cell.

    @return     pointer to a cell object
*/
Cell*
Entity::GetCell() const
{
    return this->cell;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of myself.
*/
void
Entity::RenderDebug()
{
    nGfxServer2::Instance()->DrawShape(nGfxServer2::Box, this->shadowBox.to_matrix44(), this->GetCell()->GetDebugColor());
}

//------------------------------------------------------------------------------
/**
    Extend the current global bounding box away from the lights which currently
    light this object. Light links must exist. This is necessary for
    proper shadow rendering (so that shadows don't pop in and out of view
    because the object isn't visible).
*/
const bbox3&
Entity::GetShadowBox()
{
    // check if we are already uptodate for this frame...
    uint curFrameId = Graphics::Server::Instance()->GetFrameId();
    if (curFrameId != this->shadowBoxUpdatedFrameId)
    {
        this->shadowBoxUpdatedFrameId = curFrameId;

        // get current global bounding box and extend it for each light source
        this->shadowBox = this->GetBox();

        const float directionalExtrudeLength = this->shadowBox.size().y;
        const vector3& myPos = this->GetTransform().pos_component();

        // for each light link...
        vector3 lightVec, extrudeVec;
        int i;
        int num = this->GetNumLinks(LightLink);
        for (i = 0; i < num; i++)
        {
            LightEntity* lightEntity = (LightEntity*) this->GetLinkAt(LightLink, i);
            n_assert(lightEntity->IsA(LightEntity::RTTI));
            if (lightEntity->GetLight().GetCastShadows())
            {
                const nLight& light = lightEntity->GetLight();
                nLight::Type lightType = light.GetType();
                if (nLight::Directional == lightType)
                {
                    // handle directional light
                    lightVec = lightEntity->GetTransform().z_component();
                    lightVec.norm();
                    extrudeVec = lightVec * directionalExtrudeLength;
                    this->shadowBox.extend(this->shadowBox.vmin - extrudeVec);
                    this->shadowBox.extend(this->shadowBox.vmax - extrudeVec);
                }
                else if (nLight::Point == lightType)
                {
                    // compute the vector from light source to object
                    const vector3& lightPos = lightEntity->GetTransform().pos_component();
                    lightVec = myPos - lightPos;
                    lightVec.norm();
                    extrudeVec = lightVec * light.GetRange();
                    this->shadowBox.extend(lightPos + extrudeVec);
                }
                else
                {
                    // FIXME: unsupported light type
                }
            }
        }
        return this->shadowBox;
    }
    else
    {
        // NOTE: stupid construct, but nice for setting a break point
        return this->shadowBox;
    }
}

} // namespace Graphics