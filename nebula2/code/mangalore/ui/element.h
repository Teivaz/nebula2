#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H
//------------------------------------------------------------------------------
/**
    @class UI::Element

    The base class for all 2D user interface elements describes a rectangle
    and implements a hierarchy.

    (C) 2005 Radon Labs GmbH
*/
#include "message/port.h"
#include "input/ninputevent.h"
#include "mathlib/rectangle.h"
#include "scene/ntransformnode.h"

//------------------------------------------------------------------------------
namespace UI
{
class Element : public Message::Port
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
    /// set visible flag of the element
    void SetVisible(bool b);
    /// get visible flag of the element
    bool IsVisible() const;
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
    /// get my own view space matrix
    virtual matrix44 GetViewSpaceTransform() const;
    /// set view space transform
    virtual void SetViewSpaceTransform(const matrix44& m);
    /// get current screen space rectangle (0.0 .. 1.0)
    const rectangle& GetScreenSpaceRect() const;
    /// emit a gui event
    void PutEvent(const nString& eventName);
    /// set user data
    void SetUserData(void* d);
    /// get user data
    void* GetUserData() const;

    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called when gui hierarchy is destroyed
    virtual void OnDestroy();
    /// return true if element is valid (between OnCreate() and OnDestroy())
    bool IsValid() const;

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
    bool isVisible;
    rectangle screenSpaceRect;
    nClass* transformNodeClass;
    Element* parentElement;
    nRef<nTransformNode> gfxNode;
    nArray<Ptr<Element> > childElements;
    bool isValid;
    void* userData;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Element::SetUserData(void* d)
{
    this->userData = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void*
Element::GetUserData() const
{
    return this->userData;
}

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
bool
Element::IsVisible() const
{
    return this->isVisible;
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

} // namespace UI
//------------------------------------------------------------------------------
#endif
