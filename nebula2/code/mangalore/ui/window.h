#ifndef UI_WINDOW_H
#define UI_WINDOW_H
//------------------------------------------------------------------------------
/**
    @class UI::Window

    Base class for windows. A window generally has exactly one canvas
    attached and a virtual method to handle the events for the window.

    Override this class to implement windows with new behaviour.
    The window class generally implements only functionality, not layout!

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "ui/windoweventhandler.h"
#include "ui/canvas.h"

//------------------------------------------------------------------------------
namespace UI
{
class Window : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Window);
public:
    /// constructor
    Window();
    /// destructor
    virtual ~Window();
    /// set the gfx resource for the window
    void SetResource(const nString& res);
    /// get the gfx resource for the window
    const nString& GetResource() const;
    /// open the window (will attach the window to the UI server)
    virtual void Open();
    /// close the window (will remove the window from the UI server)
    virtual void Close();
    /// close window, if currently inside event handler
    void CloseFromEventHandler();
    /// return true if window is open
    bool IsOpen() const;
    /// called to handle a gui event
    virtual void HandleEvent(Event* e);

    /// called once per frame after input has been delivered
    virtual void OnFrame();
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
    /// called when right mouse button is released over element
    virtual void OnRightButtonUp(const vector2& mousePos);
    /// called when a character is input
    virtual void OnChar(uchar charCode);
    /// called when a key is pressed
    virtual void OnKeyDown(nKey key);
    /// called when a key is released
    virtual void OnKeyUp(nKey key);
    /// return true if mouse is within element area
    virtual bool Inside(const vector2& mousePos);

private:
    nString resName;
    Ptr<Canvas> canvas;
    Ptr<WindowEventHandler> eventHandler;
    bool isOpen;
    bool closedFromEventHandler;
};
RegisterFactory(Window);

//------------------------------------------------------------------------------
/**
*/
inline
bool
Window::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Window::SetResource(const nString& res)
{
    this->resName = res;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Window::GetResource() const
{
    return this->resName;
}

} // namespace UI
//------------------------------------------------------------------------------
#endif
