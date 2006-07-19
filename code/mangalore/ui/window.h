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
#include "message/port.h"
#include "ui/windoweventhandler.h"
#include "ui/canvas.h"
#include "graphics/resource.h"

//------------------------------------------------------------------------------
namespace UI
{
class Window : public Message::Port
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
    /// set optional external event handler
    void SetEventHandler(EventHandler* h);
    /// get external event handler (can be 0)
    EventHandler* GetEventHandler() const;

    /// optionally pre-load graphics resources
    virtual void PreloadResource();
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
    /// get the canvas of the window
    Canvas* GetCanvas() const;

    /// called once per frame after input has been delivered
    virtual void OnFrame();
    /// called before the gui hierarchy is rendered
    virtual void OnRender();
    /// return true if mouse is within element area
    virtual bool Inside(const vector2& mousePos);

protected:
    nString resName;
    Ptr<Canvas> canvas;
    Ptr<EventHandler> extEventHandler;
    Ptr<Graphics::Resource> gfxResource;
    bool isOpen;
    bool closedFromEventHandler;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Window::SetEventHandler(EventHandler* h)
{
    n_assert(h);
    this->extEventHandler = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
EventHandler*
Window::GetEventHandler() const
{
    return this->extEventHandler.get_unsafe();
}

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

//------------------------------------------------------------------------------
/**
*/
inline
Canvas*
Window::GetCanvas() const
{
    return this->canvas;
}

} // namespace UI
//------------------------------------------------------------------------------
#endif
