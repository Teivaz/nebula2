#ifndef UI_CONTROL_H
#define UI_CONTROL_H
//------------------------------------------------------------------------------
/**
    @class UI::Control

    Base class for interactive UI elements.

    (C) 2005 Radon Labs GmbH
*/
#include "ui/element.h"

namespace Message
{
    class Msg;
};

namespace Input
{
    class Mapping;
}

//------------------------------------------------------------------------------
namespace UI
{
class Control : public Element
{
    DeclareRtti;
	DeclareFactory(Control);

public:
    /// constructor
    Control();
    /// destructor
    virtual ~Control();
    /// enable/disable the control
    void SetEnabled(bool b);
    /// get enabled state of control
    bool IsEnabled() const;
    
    /// set optional tooltip string
    void SetTooltip(const nString& tooltip);
    /// get optional tooltip string
    const nString& GetTooltip() const;
    /// set delay until tooltip will be shown
    void SetTooltipDelay(nTime delay);
    /// get delay until tooltip will be shown
    nTime GetTooltipDelay() const;
    
    /// set left navigation element
    void SetNavLeft(const nString& e);
    /// get left navigation element
    const nString& GetNavLeft() const;
    /// set right navigation element
    void SetNavRight(const nString& e);
    /// get right navigation element
    const nString& GetNavRight() const;
    /// set up navigation element
    void SetNavUp(const nString& e);
    /// get up navigation element
    const nString& GetNavUp() const;
    /// set down navigation element
    void SetNavDown(const nString& e);
    /// get down navigation element
    const nString& GetNavDown() const;
    
    /// called when gui hierarchy is created
    virtual void OnCreate(Element* parent);
    /// called when gui hierarchy is destroyed
    virtual void OnDestroy();
    /// called once per frame after input has been delivered
    virtual void OnFrame();

    /// accept input events 
    virtual bool Accepts(Message::Msg* msg);
    /// handle input events
    virtual void HandleMessage(Message::Msg* msg);

    /// called when mouse is moved
    virtual bool OnMouseMove(const vector2& mousePos);
    /// called when left mouse button is pressed over element
    virtual bool OnLeftButtonDown(const vector2& mousePos);
    /// called when left mouse button is released over element
    virtual bool OnLeftButtonUp(const vector2& mousePos);
    /// called when right mouse button is pressed over element
    virtual bool OnRightButtonDown(const vector2& mousePos);
    /// called when right mouse button is released over element
    virtual bool OnRightButtonUp(const vector2& mousePos);
    /// called when a character is input
    virtual bool OnChar(uchar charCode);
    /// called when a key is pressed
    virtual bool OnKeyDown(nKey key);
    /// called when a key is released
    virtual bool OnKeyUp(nKey key);

protected:
    nString navLeft;
    nString navRight;
    nString navUp;
    nString navDown;
    bool enabled;
    nTime tooltipDelay;
    nString tooltip;
    nTime mouseWithinTime;
    bool mouseWithin;

    /// attach to input mappings
    void AttachInput();
    /// remove from input mappings
    void RemoveInput();
};

RegisterFactory(Control);

//------------------------------------------------------------------------------
/**
*/
inline
bool
Control::IsEnabled() const
{
    return this->enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetTooltip(const nString& t)
{
    this->tooltip = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Control::GetTooltip() const
{
    return this->tooltip;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetTooltipDelay(nTime d)
{
    this->tooltipDelay = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Control::GetTooltipDelay() const
{
    return this->tooltipDelay;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetNavLeft(const nString& e)
{
    this->navLeft = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Control::GetNavLeft() const
{
    return this->navLeft;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetNavRight(const nString& e)
{
    this->navRight = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Control::GetNavRight() const
{
    return this->navRight;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetNavUp(const nString& e)
{
    this->navUp = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Control::GetNavUp() const
{
    return this->navUp;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetNavDown(const nString& e)
{
    this->navDown = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Control::GetNavDown() const
{
    return this->navDown;
}

} // namespace UI
//------------------------------------------------------------------------------
#endif
