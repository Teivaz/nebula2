#ifndef UI_WINDOWEVENTHANDLER_H
#define UI_WINDOWEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class UI::WindowEventHandler
  
    The window event handler routes gui event to a window object's
    HandleEvent() method. It is created and fully controlled by
    the window.
    
    (C) 2005 Radon Labs GmbH
*/
#include "ui/eventhandler.h"

//------------------------------------------------------------------------------
namespace UI
{
class Window;

class WindowEventHandler : public EventHandler
{
    DeclareRtti;
	DeclareFactory(WindowEventHandler);
public:
    /// constructor
    WindowEventHandler();
    /// destructor
    virtual ~WindowEventHandler();
    /// override this method to handle your user interface events
    virtual void HandleEvent(Event* e);
    /// set the pointer to the Window object
    void SetWindow(Window* w);

private:
    Window* window; // NOTE: not a Ptr<> to prevent cyclic dependency!
};

//------------------------------------------------------------------------------
/**
*/
inline
void
WindowEventHandler::SetWindow(Window* w)
{
    n_assert(w);
    this->window = w;
}

}; // namespace UI
//------------------------------------------------------------------------------
#endif