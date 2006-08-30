//------------------------------------------------------------------------------
//  application/guieventhandler.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "application/guieventhandler.h"
#include "application/guistatehandler.h"

namespace Application
{
ImplementRtti(Application::GuiEventHandler, UI::EventHandler);
ImplementFactory(Application::GuiEventHandler);

//------------------------------------------------------------------------------
/**
*/
GuiEventHandler::GuiEventHandler() :
    guiStateHandler(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GuiEventHandler::~GuiEventHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Handle a user interface event. This simply routes the event to our
    event handler.
*/
void
GuiEventHandler::HandleEvent(UI::Event* e)
{
    n_assert(0 != this->guiStateHandler);
    this->guiStateHandler->OnEvent(e);
}

} // namespace Application

