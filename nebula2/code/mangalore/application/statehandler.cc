//------------------------------------------------------------------------------
//  application/statehandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/statehandler.h"

namespace Application
{
ImplementRtti(Application::StateHandler, Foundation::RefCounted);

//------------------------------------------------------------------------------
/**
*/
StateHandler::StateHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StateHandler::~StateHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the state handler is attached to the App object.
    One-time inits should be done here.
*/
void
StateHandler::OnAttachToApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the state handler is removed from
    the App object.
*/
void
StateHandler::OnRemoveFromApplication()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the state associated with this state handler
    is entered. The parameter indicates the previously active state.

    @param  prevState   previous state
*/
void
StateHandler::OnStateEnter(const nString& prevState)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the state associated with this state handler
    is left. The parameter indicates the next active state.

    @param  nextState   next state
*/
void
StateHandler::OnStateLeave(const nString& nextState)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called on the current application state after the 
    database is opened and before entities are loaded. You can use this
    method to setup managers which depend on a valid database.
*/
void
StateHandler::OnLoadBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called on the current application state after entities 
    have been loaded. You can use this method to setup managers which depend 
    on loaded entities.
*/
void
StateHandler::OnLoadAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called once a frame while the state is active. The method
    must return a state identifier. If this is different from the current
    state, a state switch will occur after the method returns. This method
    MUST be overriden by subclasses.

    @return     a state identifier
*/
nString
StateHandler::OnFrame()
{
    return "";
}

} // namespace Application