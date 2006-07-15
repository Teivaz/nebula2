//------------------------------------------------------------------------------
//  nguieventhandler.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "gui/nguieventhandler.h"

//------------------------------------------------------------------------------
/**
*/
nGuiEventHandler::nGuiEventHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiEventHandler::~nGuiEventHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiEventHandler::HandleEvent(const nGuiEvent& e)
{
    // empty, override in subclass!
}
