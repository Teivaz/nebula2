//------------------------------------------------------------------------------
//  input/chain.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/chain.h"
#include "input/event.h"

namespace Input
{
ImplementRtti(Input::Chain, Message::Dispatcher);
ImplementFactory(Input::Chain);

//------------------------------------------------------------------------------
/**
*/
void
Chain::HandleMessage(Message::Msg*)
{
    // EMPTY but needed to get private declared! Do not use! Use SendEvents instead!
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::BeginFrame()
{
    // create a begin frame event and send to all sinks
    nArray<Ptr<Event> > events;
    Ptr<Event> event = Event::Create();
    event->SetType(Event::BeginFrame);
    events.Append(event);

    this->SendEvents(events);
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::EndFrame()
{
    // create a end frame event and send to all sinks
    nArray<Ptr<Event> > events;
    Ptr<Event> event = Event::Create();
    event->SetType(Event::EndFrame);
    events.Append(event);

    this->SendEvents(events);
}

//------------------------------------------------------------------------------
/**
    Let the ports handle the message in the order of the priority.
*/
void
Chain::SendEvents(const nArray<Ptr<Event> >& events)
{
    // try to cleanup
    this->CleanupEmptyPorts();

    // lock ports array
    this->BeginHandleMessage();

    int portIndex;
    int numPorts = this->ports.Size();
    for (portIndex = 0; portIndex < numPorts; portIndex++)
    {
        if (this->ports[portIndex].port != 0)
        {
            // send all events to this port
            int eventIndex;
            for (eventIndex = 0; eventIndex < events.Size(); eventIndex++)
            {
                // check this here again, because it is possible the port dissconnects as result of event handling
                if (this->ports[portIndex].port != 0)
                {
                    const Ptr<Event>& event = events[eventIndex];

                    // set the priority of the current sink (this makes it possible
                    // that the same sink is attached twice and know when to react because of the priority)
                    event->SetPriority(this->ports[portIndex].priority);

                    if (this->ports[portIndex].port->Accepts(event))
                    {
                        this->ports[portIndex].port->HandleMessage(event);
                    }
                }
            }
        }
    }

    // unlock ports array
    this->EndHandleMessage();
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::AttachInputSink(int priority, Message::Port* sink)
{
    n_assert(sink);

    // call the cleanup to remove empty elements when not in trigger
    this->CleanupEmptyPorts();

    PriorityPort elm;
    elm.port = sink;
    elm.priority = priority;

    // find place to insert
    int insertBehind = -1;
    int i;
    for (i = 0; i < this->ports.Size(); i++)
    {
        // deal with empty ports
        if (this->ports[i].port != 0)
        {
            if (priority <= this->ports[i].priority)
            {
                insertBehind = i;
            }
            else
            {
                break;
            }
        }
        else
        {
            // skip empty port
            insertBehind = i;
        }
    }
    // insert
    if (insertBehind >= 0)
    {
        // insert (the append as last element is handled from Insert();)
        n_assert(this->ports.Size() >= insertBehind + 1);
        this->ports.Insert(insertBehind + 1, elm);
    }
    else
    {
        // add in front
        this->ports.Insert(0, elm);
    }

    // add to parent (so Accepts will work)
    Message::Dispatcher::AttachPort(sink);
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::RemoveInputSink(Message::Port* sink)
{
    n_assert(sink);
    // find element
    int i;
    for (i = 0; i < this->ports.Size(); i++)
    {
        if (this->ports[i].port.get_unsafe() == sink)
        {
            // set element to 0 (do not delete the array element, to be able to remove while trigger)
            this->ports[i].port = 0;
            // remove from parent
            Message::Dispatcher::RemovePort(sink);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::RemoveInputSink(int priority, Message::Port* sink)
{
    n_assert(sink);
    // find element
    int i;
    for (i = 0; i < this->ports.Size(); i++)
    {
        if (this->ports[i].port.get_unsafe() == sink)
        {
            if (this->ports[i].priority == priority)
            {
                // set element to 0 (do not delete the array element, to be able to remove while trigger)
                this->ports[i].port = 0;
                // remove from parent
                Message::Dispatcher::RemovePort(sink);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Chain::CleanupEmptyPorts()
{
    if (!this->IsInHandleMessage())
    {
        // find empty ports and remove them
        int i;
        for (i = 0; i < this->ports.Size(); /*empty*/)
        {
            if (this->ports[i].port == 0)
            {
                this->ports.Erase(i);
            }
            else
            {
                // next
                i++;
            }
        }
    }

    // call parent
    Dispatcher::CleanupEmptyPorts();
}

//------------------------------------------------------------------------------
/**
*/
int
Chain::GetNumSinks() const
{
    int count = 0;
    int i;
    for (i = 0; i < this->ports.Size(); i++)
    {
        if (this->ports[i].port != 0)
        {
            count++;
        }
    }
    return count;
}


}; // namespace Input