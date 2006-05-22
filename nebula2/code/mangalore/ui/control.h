#ifndef UI_CONTROL_H
#define UI_CONTROL_H
//------------------------------------------------------------------------------
/**
    @class UI::Control

    Base class for interactive UI elements.
    
    (C) 2005 Radon Labs GmbH
*/
#include "ui/element.h"

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

protected:
    nString navLeft;
    nString navRight;
    nString navUp;
    nString navDown;
    bool enabled;
};

RegisterFactory(Control);

//------------------------------------------------------------------------------
/**
*/
inline
void
Control::SetEnabled(bool b)
{
    this->enabled = b;
}

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

}; // namespace UI
//------------------------------------------------------------------------------
#endif
    
