#ifndef INPUT_CHAIN_H
#define INPUT_CHAIN_H
//------------------------------------------------------------------------------
/**
    @class Input::Chain

    A list of Msg::Ports attached to this Dispatcher, ordered by Priority.

    (C) 2006 RadonLabs GmbH
*/
#include "message/dispatcher.h"

namespace Input
{
class Event;

class Chain : public Message::Dispatcher
{
    DeclareRtti;
	DeclareFactory(Chain);

public:

    /// begin input frame
    void BeginFrame();
    /// end input frame
    void EndFrame();

    // ---- attach/remove input sinks to this chain
    /// attach a input sink, the priority defines the order in the chain
    void AttachInputSink(int priority, Message::Port* sink);
    /// remove a input sink (from all priorities)
    void RemoveInputSink(Message::Port* sink);
    /// remove the input sink from the given priority
    void RemoveInputSink(int priority, Message::Port* sink);

    /// send this array of events
    void SendEvents(const nArray<Ptr<Event> >& events);

    /// get number of attached input sinks
    int GetNumSinks() const;

private:
    /// private: do not use, use SendEvents instead!
    virtual void HandleMessage(Message::Msg* msg);
    /// cleanup empty ports when not in trigger
    virtual void CleanupEmptyPorts();

    struct PriorityPort
    {
        int priority;
        Ptr<Message::Port> port;
    };

    nArray<PriorityPort> ports;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif
