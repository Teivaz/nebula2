#ifndef UI_EVENTHANDLER_H
#define UI_EVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class UI::EventHandler

    An UI event handler implements the logic behind an user interface. It is
    implemented as a Message::Port which accepts messages of the type
    UI::Event. Derive a subclass to implement your UI logic inside
    the HandleEvent() method.
*/
#include "message/port.h"

//------------------------------------------------------------------------------
namespace UI
{
class Event;

class EventHandler : public Message::Port
{
    DeclareRtti;
	DeclareFactory(EventHandler);

public:
    /// constructor
    EventHandler();
    /// destructor
    virtual ~EventHandler();
    /// override this method to handle your user interface events
    virtual void HandleEvent(Event* e);

protected:
    /// only accept UI::Event messages
    virtual bool Accepts(Message::Msg* msg);
    /// this simply routes the message to HandleEvent()!
    virtual void HandleMessage(Message::Msg* msg);
};

RegisterFactory(EventHandler);

} // namespace UI
//------------------------------------------------------------------------------
#endif
