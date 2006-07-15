#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H
//------------------------------------------------------------------------------
/**
    @class UI::Element

    The base class for all 2D user interface elements describes a rectangle
    and implements a hierarchy.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "foundation/ptr.h"
#include "input/ninputevent.h"
#include "mathlib/rectangle.h"
#include "scene/ntransformnode.h"

//------------------------------------------------------------------------------
namespace UI
{
class Element : public Foundation::RefCounted
{
    DeclareRtti;
public:
    /// constructor
    Element();
    /// destructor
    virtual ~Element();
    /// set id
    void SetId(const nString& i);
    /// get id
    const nString& GetId() const;
    /// set dismissed flag on the element
    void SetDismissed(bool b);
    /// get dismissed flag of the element
    bool IsDismissed() const;
    /// set Nebula node associated with the element
    void SetGfxNode(nTransformNode* n);
    /// get Nebula node associated with the element
    nTransformNode* GetGfxNode() const;
    /// attach child element
    void AttachElement(Element* elm);
    /// remove child element
    void RemoveElement(Element* elm);
    /// access to child elements
    const nArray<Ptr<Element> >& GetElements() const;
    /// find child element by id
    Element* FindElement(const nString& i);
    /// get pointer to parent element (can be 0)
    Element* GetParent() const;
    /// get current screen space rectangle (0.0 .. 1.0)
    const rectangle& GetScreenSpaceRect() const;
    /// emit a gui event
    void PutEvent(const nString& eventName);
    /// set optional tooltip string
    void SetTooltip(const nString& tooltip);
    /// get optional tooltip string
    const nString& GetTooltip() const;
    /// set delay until tooltip will be shown
    void SetTooltipDelay(nTime delay);
    /// get delay until tooltip will be shown
    nTime GetTooltipDelay() const;

    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called when gui hierarchy is destroyed
    virtual void OnDestroy();
    /// return true if element is valid (between OnCreate() and OnDestroy())
    bool IsValid() const;
    /// called when mouse is moved
    virtual void OnMouseMove(const vector2& mousePos);
    /// called when left mouse button is pressed over element
    virtual void OnLeftButtonDown(const vector2& mousePos);
    /// called when left mouse button is released over element
    virtual void OnLeftButtonUp(const vector2& mousePos);
    /// called when right mouse button is pressed over element
    virtual void OnRightButtonDown(const vector2& mousePos);
    /// called when right mouse button is released over element
    virtual void OnRightButtonUp(const vector2& mousePos);
    /// called when a character is input
    virtual void OnChar(uchar charCode);
    /// called when a key is pressed
    virtual void OnKeyDown(nKey key);
    /// called when a key is released
    virtual void OnKeyUp(nKey key);
    /// called once per frame after input has been delivered
    virtual void OnFrame();
    /// called before the gui hierarchy is rendered
    virtual void OnRender();
    /// return true if mouse is within element area
    virtual bool Inside(const vector2& mousePos);

protected:
    friend class FactoryManager;
    /// update the screen space rectangle
    void UpdateScreenSpaceRect();

protected:
    nString id;
    bool dismissed;
    rectangle screenSpaceRect;
    nClass* transformNodeClass;
    Element* parentElement;
    nRef<nTransformNode> gfxNode;
    nArray<Ptr<Element> > childElements;

    nTime tooltipDelay;
    nString tooltip;
    nTime mouseWithinTime;
    bool mouseWithin;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
Element::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetId(const nString& i)
{
    this->id = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Element::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetDismissed(bool b)
{
    this->dismissed = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Element::IsDismissed() const
{
    return this->dismissed;
}

//------------------------------------------------------------------------------
/**
    Get pointer to parent element. This method may return 0 if no parent
    element exists.
*/
inline
Element*
Element::GetParent() const
{
    return this->parentElement;
}

//------------------------------------------------------------------------------
/**
    Returns the screen space rectangle of the node.
*/
inline
const rectangle&
Element::GetScreenSpaceRect() const
{
    return this->screenSpaceRect;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetGfxNode(nTransformNode* n)
{
    n_assert(n);
    this->gfxNode = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTransformNode*
Element::GetGfxNode() const
{
    return this->gfxNode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetTooltip(const nString& tooltip)
{
    this->tooltip = tooltip;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Element::GetTooltip() const
{
    return this->tooltip;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetTooltipDelay(nTime delay)
{
    n_assert(delay >= 0.0);
    this->tooltipDelay = delay;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Element::GetTooltipDelay() const
{
    return this->tooltipDelay;
}


} // namespace UI
//------------------------------------------------------------------------------
#endif
