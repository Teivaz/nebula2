//------------------------------------------------------------------------------
//  ui/window.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/window.h"
#include "ui/server.h"
#include "ui/event.h"

namespace UI
{
ImplementRtti(UI::Window, Foundation::RefCounted);
ImplementFactory(UI::Window);

//------------------------------------------------------------------------------
/**
*/
Window::Window() :
    isOpen(false),
    closedFromEventHandler(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Window::~Window()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
    This method can be used before Open() to preload the graphics resources
    required by the window.
*/
void
Window::PreloadResource()
{
    n_assert(!this->gfxResource.isvalid());
    this->gfxResource = Graphics::Resource::Create();
    this->gfxResource->SetName(this->resName);
    this->gfxResource->Load();
}

//------------------------------------------------------------------------------
/**
    Opens the window. This will register the window with the
    UI::Server, make the window visible and start to process UI
    messages.
*/
void
Window::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->resName.IsValid());
    this->isOpen = true;
    this->closedFromEventHandler = false;

    // create the canvas for the window
    this->canvas = Canvas::Create();
    this->canvas->SetResourceName(this->resName);
    this->canvas->OnCreate(0);

    // attach the window to the UI::Server
    UI::Server::Instance()->AttachWindow(this);
}

//------------------------------------------------------------------------------
/**
    Closes the window. This will make the window invisible, stop processing
    UI events and unregister the window from the UI::Server.
*/
void
Window::Close()
{
    n_assert(this->IsOpen());
    this->isOpen = false;
    this->closedFromEventHandler = false;
    this->canvas->OnDestroy();
    this->canvas = 0;
    UI::Server::Instance()->RemoveWindow(this);
}

//------------------------------------------------------------------------------
/**
    Use this method if the window is closed from within the event handler.
    This will actually close the window after event handling has happened,
    so that a valid object is available during the event handling!
*/
void
Window::CloseFromEventHandler()
{
    this->closedFromEventHandler = true;
}

//------------------------------------------------------------------------------
/**
    This method is called by this window's event handler when an event 
    should be processed. You can override this method if you are 
    interested in events emitted by elements in this window.
*/
void
Window::HandleEvent(Event* e)
{
    n_assert(e);
    n_printf("Window::HandleEvent: %s\n", e->GetEventName().Get());
    if (this->extEventHandler.isvalid())
    {
        this->extEventHandler->HandleEvent(e);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called per-frame by the UI::Server as long as the
    window is open. You may override the method to implement
    your own per-frame stuff there.
*/
void
Window::OnFrame()
{
    if (this->closedFromEventHandler)
    {
        this->Close();
    }
    else if (this->IsOpen())
    {
        this->canvas->OnFrame();
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by the UI::Server when the window should
    render itself.
*/
void
Window::OnRender()
{
    if (this->IsOpen())
    {
        this->canvas->OnRender();
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the mouse is within the window.
*/
bool
Window::Inside(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        return this->canvas->Inside(mousePos);
    }
    else
    {
        return false;
    }
}

} // namespace UI
