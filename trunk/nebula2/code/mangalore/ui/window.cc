//------------------------------------------------------------------------------
//  ui/window.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/window.h"
#include "ui/server.h"

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

    // create event handler
    this->eventHandler = WindowEventHandler::Create();
    this->eventHandler->SetWindow(this);

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
    this->closedFromEventHandler = false;
    this->eventHandler = 0;
    this->canvas->OnDestroy();
    this->canvas = 0;
    UI::Server::Instance()->RemoveWindow(this);
    this->isOpen = false;
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
    // implement in subclass!
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
    Called by UI::Server when the mouse is moved.
*/
void
Window::OnMouseMove(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        this->canvas->OnMouseMove(mousePos);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when the left mouse button is pressed.
*/
void
Window::OnLeftButtonDown(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        this->canvas->OnLeftButtonDown(mousePos);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when the left mouse button is released.
*/
void
Window::OnLeftButtonUp(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        this->canvas->OnLeftButtonUp(mousePos);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when the right mouse button is pressed.
*/
void
Window::OnRightButtonDown(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        this->canvas->OnRightButtonDown(mousePos);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when the right mouse button is released.
*/
void
Window::OnRightButtonUp(const vector2& mousePos)
{
    if (this->IsOpen())
    {
        this->canvas->OnRightButtonUp(mousePos);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when a character has been input.
*/
void
Window::OnChar(uchar charCode)
{
    if (this->IsOpen())
    {
        this->canvas->OnChar(charCode);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when a key has been pressed.
*/
void
Window::OnKeyDown(nKey key)
{
    if (this->IsOpen())
    {
        this->canvas->OnKeyDown(key);
    }
}

//------------------------------------------------------------------------------
/**
    Called by UI::Server when a key has been released.
*/
void
Window::OnKeyUp(nKey key)
{
    if (this->IsOpen())
    {
        this->canvas->OnKeyUp(key);
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

}; // namespace UI
