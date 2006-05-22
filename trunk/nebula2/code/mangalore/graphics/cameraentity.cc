//------------------------------------------------------------------------------
//  graphics/cameraentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/cameraentity.h"
#include "gfx2/ngfxserver2.h"

namespace Graphics
{
ImplementRtti(Graphics::CameraEntity, Graphics::Entity);
ImplementFactory(Graphics::CameraEntity);

//------------------------------------------------------------------------------
/**
*/
CameraEntity::CameraEntity() :
    camera(60.0f, 4.0f / 3.0f, 0.1f, 5000.0f),
    viewProjDirty(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CameraEntity::~CameraEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Entity::Type
CameraEntity::GetType() const
{
    return Camera;
}

//------------------------------------------------------------------------------
/**
    Updates the entity's bounding box. Since this is a camera there is no
    graphics resource attached which would normally deliver the
    bounding box.
*/
void
CameraEntity::OnActivate()
{
    Entity::OnActivate();
    this->SetLocalBox(this->camera.GetBox());
}

//------------------------------------------------------------------------------
/**
*/
void
CameraEntity::SetTransform(const matrix44& m)
{
    this->viewProjDirty = true;
    Entity::SetTransform(m);
}

//------------------------------------------------------------------------------
/**
    Get clip status of bounding box against this entity. I.e.: Clipped
    means, the boxes intersect. Outside means, the argument box is outside
    my clip space, Inside means, the argument box is inside my clip space.
*/
Entity::ClipStatus
CameraEntity::GetBoxClipStatus(const bbox3& box)
{
    if (this->viewProjDirty)
    {
        this->UpdateViewProjection();
    }
    bbox3::ClipStatus clipCode = box.clipstatus(this->viewProjMatrix);
    switch (clipCode)
    {
        case bbox3::Clipped:    return Clipped;
        case bbox3::Inside:     return Inside;
        default:                return Outside;
    }
}

//------------------------------------------------------------------------------
/**
    Get clip status of other view volume against this entity:

    Clipped:    the two view volumes touch
    Outside:    the two view volumes do not touch
    Inside:     I am completely inside the other view volume
*/
Entity::ClipStatus
CameraEntity::GetViewVolumeClipStatus(const matrix44& transform, nCamera2& camera)
{
    if (this->viewProjDirty)
    {
        this->UpdateViewProjection();
    }
    nCamera2::ClipStatus clipCode = camera.GetClipStatus(transform, this->viewProjMatrix);
    switch (clipCode)
    {
        case nCamera2::Clipped: return Clipped;
        case nCamera2::Inside:  return Inside;
        default:                return Outside;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the current view projection matrix.
*/
const matrix44&
CameraEntity::GetViewProjection()
{
    if (this->viewProjDirty)
    {
        this->UpdateViewProjection();
    }
    return this->viewProjMatrix;
}

//------------------------------------------------------------------------------
/**
    Change camera characteristics. Note that this also updates the
    entity's bounding box and may change the entity's position the
    cell tree.
*/
void
CameraEntity::SetCamera(const nCamera2& cam)
{
    this->viewProjDirty = true;
    this->camera = cam;
    this->SetLocalBox(this->camera.GetBox());
    this->UpdatePositionInCellTree();
}

//------------------------------------------------------------------------------
/**
    This method is called before the Render() method is invoked on the entity.
    The camera entity simply distributes the method on all linked graphics
    entities.
*/
void 
CameraEntity::OnRenderBefore()
{
    // update Nebula2's camera
    nGfxServer2::Instance()->SetCamera(this->camera);

    // invoke OnRenderBefore() on entities visible by this camera
    int numLinks = this->GetNumLinks(CameraLink);
    int linkIndex;
    for (linkIndex = 0; linkIndex < numLinks; linkIndex++)
    {
        Entity* link = this->GetLinkAt(CameraLink, linkIndex);
        n_assert(link && (link != this));
        link->OnRenderBefore();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after rendering has happened, and the
    scene has been presented.
    The camera entity simply distributes the method on all linked graphics
    entities.
*/
void
CameraEntity::OnRenderAfter()
{
    int numLinks = this->GetNumLinks(CameraLink);
    int linkIndex;
    for (linkIndex = 0; linkIndex < numLinks; linkIndex++)
    {
        Entity* link = this->GetLinkAt(CameraLink, linkIndex);
        n_assert(link && (link != this));
        link->OnRenderAfter();
    }
}

//------------------------------------------------------------------------------
/**
    This simply renders all linked entities. All entities that are
    visible by this camera have been linked by the Cell::UpdateVisibility()
    process.
*/
void
CameraEntity::Render()
{
    int numLinks = this->GetNumLinks(CameraLink);
    int linkIndex;
    for (linkIndex = 0; linkIndex < numLinks; linkIndex++)
    {
        Entity* link = this->GetLinkAt(CameraLink, linkIndex);
        n_assert(link && (link != this) && (link->GetVisible()));
        link->Render();
    }
}

} // namespace Graphics
