#ifndef UI_BUTTON_H
#define UI_BUTTON_H
//------------------------------------------------------------------------------
/**
    @class UI::Button
    
    A simple button UI element.

    (C) 2005 Radon Labs GmbH
*/
#include "ui/control.h"

//------------------------------------------------------------------------------
namespace UI
{
class Button : public Control
{
    DeclareRtti;
	DeclareFactory(Button);

public:
    /// constructor
    Button();
    /// destructor
    virtual ~Button();
    /// set button text
    void SetText(const nString& t);
    /// get button text
    const nString& GetText() const;
    /// set event name
    void SetEventName(const nString& e);
    /// get event name
    const nString& GetEventName() const;
    /// return true if currently pressed
    bool IsPressed() const;
    /// return true if mouse currently over button
    bool IsMouseOver() const;
    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called before the gui hierarchy is rendered
    virtual void OnRender();
    /// called when mouse is moved
    virtual void OnMouseMove(const vector2& mousePos);
    /// called when left mouse button is pressed over element
    virtual void OnLeftButtonDown(const vector2& mousePos);
    /// called when left mouse button is released over element
    virtual void OnLeftButtonUp(const vector2& mousePos);
    /// called when right mouse button is pressed over element
    virtual void OnRightButtonDown(const vector2& mousePos);

protected:
    /// control states
    enum VisualState
    {
        Normal,
        Pressed,
        MouseOver,
        Disabled,

        NumVisualStates,
    };

    nString text;
    nString eventName;
    bool mouseOver;
    bool pressed;
    nFixedArray<nRef<nTransformNode> > visuals;
};

RegisterFactory(Button);

//------------------------------------------------------------------------------
/**
*/
inline
void
Button::SetText(const nString& t)
{
    this->text = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Button::GetText() const
{
    return this->text;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Button::SetEventName(const nString& e)
{
    this->eventName = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Button::GetEventName() const
{
    return this->eventName;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Button::IsPressed() const
{
    return this->pressed;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Button::IsMouseOver() const
{
    return this->mouseOver;
}

}; // namespace UI
//------------------------------------------------------------------------------
#endif
