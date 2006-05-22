//------------------------------------------------------------------------------
//  ui/eventhandler.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/eventhandler.h"
#include "ui/event.h"

namespace UI
{
ImplementRtti(UI::EventHandler, Message::Port);
ImplementFactory(UI::EventHandler);

//------------------------------------------------------------------------------
/**
*/
EventHandler::EventHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EventHandler::~EventHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Only accepts UI::Event messages.
*/
bool
EventHandler::Accepts(Message::Msg* msg)
{
    return msg->CheckId(UI::Event::Id);
}

//------------------------------------------------------------------------------
/**
    The HandleMessage() method makes sure the incoming message is an
    UI::Event and routes the method to HandleEvent(). In your subclass,
    HandleEvent() should be overriden to implement your UI logic.
*/
void
EventHandler::HandleMessage(Message::Msg* msg)
{
    n_assert(msg->CheckId(UI::Event::Id));
    this->HandleEvent((UI::Event*)msg);
}

//------------------------------------------------------------------------------
/**
    The HandleEvent() method is the front end for your UI logic implementation.
    All user interface events emitted by the current UI will be routed to
    this method.
*/
void
EventHandler::HandleEvent(Event* event)
{
    // empty (implement in subclass)
}

}; // namespace UI
