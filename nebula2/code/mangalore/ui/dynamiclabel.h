#ifndef UI_DYNAMICLABEL_H
#define UI_DYNAMICLABEL_H
//------------------------------------------------------------------------------
/**
    A GUI label with its own GraphicsEntity, that can be created and placed on
    a canvas at run-time.

    To create a dynamic label, you need a nebula scene with a Label UI node
    WITHOUT A CANVAS! Otherwise, the real canvas is discarded and replaced by
    then Label's scene canvas.

    (C) 2006 Radon Labs GmbH
*/
#include "ui/label.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace UI
{
class DynamicLabel
    : public Label
{
    DeclareRtti;
	DeclareFactory(DynamicLabel);

public:
    /// constructor
    DynamicLabel();
    /// destructor
    virtual ~DynamicLabel();

    /// set gfx resource name (scene should NOT contain a canvas!)
    void SetResourceName(const nString& n);
    /// get gfx resource name
    const nString& GetResourceName() const;

    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called when gui hierarchy is destroyed
    virtual void OnDestroy();
    /// called before the gui hierarchy is rendered
    virtual void OnRender();

    /// get graphics entity of canvas
    Graphics::Entity* GetGraphicsEntity() const;

    /// get my own view space matrix
    virtual matrix44 GetViewSpaceTransform() const;
    /// set view space transform
    virtual void SetViewSpaceTransform(const matrix44& m);

private:
    /// recursively find the first label node in the hierarchy
    nTransformNode* FindFirstLabelNodeInHierarchy(nTransformNode* root);

    nString resourceName;
    Ptr<Graphics::Entity> graphicsEntity;
};

RegisterFactory(DynamicLabel);

//------------------------------------------------------------------------------
/**
    NOTE: this method may return 0!
*/
inline
Graphics::Entity*
DynamicLabel::GetGraphicsEntity() const
{
    return this->graphicsEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    The scene should not contain a canvas!
*/
inline
void
DynamicLabel::SetResourceName(const nString& n)
{
    this->resourceName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
DynamicLabel::GetResourceName() const
{
    return this->resourceName;
}

}; // namespace UI

//------------------------------------------------------------------------------
#endif
