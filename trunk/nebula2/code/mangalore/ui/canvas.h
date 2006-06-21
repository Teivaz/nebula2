#ifndef UI_CANVAS_H
#define UI_CANVAS_H
//------------------------------------------------------------------------------
/**
    @class UI::Canvas

    A canvas is the toplevel object of a ui hierarchy.

    (C) 2005 Radon Labs GmbH
*/
#include "ui/element.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace UI
{
class Canvas : public Element
{
    DeclareRtti;
	DeclareFactory(Canvas);

public:
    /// constructor
    Canvas();
    /// destructor
    virtual ~Canvas();
    /// set gfx resource name
    void SetResourceName(const nString& n);
    /// get gfx resource name
    const nString& GetResourceName() const;
    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called when gui hierarchy is destroyed
    virtual void OnDestroy();
    /// called before the gui hierarchy is rendered
    virtual void OnRender();

private:
    /// recursively find the first canvas node in the hierarchy
    nTransformNode* FindCanvasNodeInHierarchy(nTransformNode* root);

    nString resourceName;
    Ptr<Graphics::Entity> graphicsEntity;
};

RegisterFactory(Canvas);

//------------------------------------------------------------------------------
/**
*/
inline
void
Canvas::SetResourceName(const nString& n)
{
    this->resourceName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Canvas::GetResourceName() const
{
    return this->resourceName;
}

} // namespace UI
//------------------------------------------------------------------------------
#endif
