//------------------------------------------------------------------------------
//  ui/windoweventhandler.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/windoweventhandler.h"
#include "ui/window.h"

namespace UI
{
ImplementRtti(UI::WindowEventHandler, UI::EventHandler);
ImplementFactory(UI::WindowEventHandler);

//------------------------------------------------------------------------------
/**
*/
WindowEventHandler::WindowEventHandler() :
    window(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WindowEventHandler::~WindowEventHandler()
{
    this->window = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
WindowEventHandler::HandleEvent(Event* e)
{
    n_assert(this->window);
    n_assert(e);
    this->window->HandleEvent(e);
}

}; // namespace UI

