#ifndef GRAPHICS_CAMERAENTITY_H
#define GRAPHICS_CAMERAENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::CameraEntity

    A camera entity can check shape entities for visibility and
    establishes a link between visible shape entities and the camera entity.
    A camera can be made the current camera in the level it is attached to.
    Only one camera entity can be current at any time.

    (C) 2003 RadonLabs GmbH
*/
#include "graphics/entity.h"
#include "gfx2/ncamera2.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class CameraEntity : public Entity
{
    DeclareRtti;
	DeclareFactory(CameraEntity);

public:
    /// constructor
    CameraEntity();
    /// destructor
    virtual ~CameraEntity();
    /// get entity type
    virtual Entity::Type GetType() const;
    /// called when attached to game entity
    virtual void OnActivate();
    /// update the entity's transform
    virtual void SetTransform(const matrix44& m);
    /// get clip status against bounding box
    virtual ClipStatus GetBoxClipStatus(const bbox3& box);
    /// get clip status against view volume
    virtual ClipStatus GetViewVolumeClipStatus(const matrix44& transform, nCamera2& camera);
    /// called before rendering happens
    virtual void OnRenderBefore();
    /// called after rendering has happened
    virtual void OnRenderAfter();
    /// render the camera, this renders all objects visible to this camera
    virtual void Render();
    /// set camera attributes (updates bounding box)
    void SetCamera(const nCamera2& cam);
    /// get camera attributes
    nCamera2& GetCamera();
    /// return the current view projection matrix
    const matrix44& GetViewProjection();
    /// get the inverse transform (== view matrix)
    const matrix44& GetView();

private:
    /// update the view projection matrix
    void UpdateViewProjection();

    nCamera2 camera;            // the Nebula2 camera definition
    matrix44 viewProjMatrix;    // the current view projection matrix
    matrix44 viewMatrix;        // the current view matrix
    bool viewProjDirty;         // dirty flag for view projection matrix
};

RegisterFactory(CameraEntity);

//------------------------------------------------------------------------------
/**
*/
inline
nCamera2&
CameraEntity::GetCamera()
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
    Updates the view and view projection matrix and clears the viewProjDirty flag.
*/
inline
void
CameraEntity::UpdateViewProjection()
{
    n_assert(this->viewProjDirty);
    this->viewProjDirty = false;
    this->viewMatrix = this->GetTransform();
    this->viewMatrix.invert_simple();
    this->viewProjMatrix = this->viewMatrix;
    this->viewProjMatrix *= this->camera.GetProjection();
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
