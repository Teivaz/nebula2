#ifndef UI_EVENT_H
#define UI_EVENT_H
//------------------------------------------------------------------------------
/**
    @class UI::Event
    
    User interface events are emitted by user interface and handled by the
    currently registered UI event handler. UIs usually consist of 2 parts,
    the user interface itself (a hierarchy of UI::Element objects, constructed
    from an XUI file) which represents the visual part, and an event handler 
    which implements the logic behind the UI.
    
    (C) 2005 Radon Labs GmbH
*/    
#include "message/msg.h"

//------------------------------------------------------------------------------
namespace UI
{
class Event : public Message::Msg
{
    DeclareRtti;
	DeclareFactory(Event);
    DeclareMsgId;

public:
    /// constructor
    Event();
    /// set event name
    void SetEventName(const nString& n);
    /// get event name
    const nString& GetEventName() const;

private:
    nString eventName;
};

RegisterFactory(Event);

//------------------------------------------------------------------------------
/**
*/
inline
Event::Event()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Event::SetEventName(const nString& n)
{
    this->eventName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Event::GetEventName() const
{
    return this->eventName;
}

}; // namespace UI
//------------------------------------------------------------------------------
#endif