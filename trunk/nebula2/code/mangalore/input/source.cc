//------------------------------------------------------------------------------
//  input/source.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/source.h"
#include "input/chain.h"
#include "input/event.h"
#include "input/mapping.h"

namespace Input
{
ImplementRtti(Input::Source, Foundation::RefCounted);
ImplementFactory(Input::Source);

//------------------------------------------------------------------------------
/**
*/
Source::Source() :
    deviceId(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Source::BeginFrame()
{
    // cleanup the event refs of the last frame
    this->events.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Source::Consume(nInputEvent* /*rawEvent*/)
{
    // nothing
}

//------------------------------------------------------------------------------
/**
*/
void
Source::Emit(Mapping* mapping)
{
    n_assert(mapping);

    if (this->events.Size() > 0)
    {
        // emit events of last frame
        mapping->SendEvents(this->events);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Source::EndFrame()
{
    // nothing
}

//------------------------------------------------------------------------------
/**
*/
void
Source::NotifyNumSinks(int num)
{
    // nothing
}

//------------------------------------------------------------------------------
/**
*/
int
Source::GetDeviceId() const
{
    return this->deviceId;
}

//------------------------------------------------------------------------------
/**
*/
void
Source::SetDeviceId(int id)
{
    this->deviceId = id;
}

//------------------------------------------------------------------------------
/**
*/
float
Source::GetAxisValue() const
{
    n_error("VIRTUAL!");
    return 0.f;
}

//------------------------------------------------------------------------------
/**
*/
bool
Source::GetButtonDown() const
{
    n_error("VIRTUAL!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Source::GetButtonUp() const
{
    n_error("VIRTUAL!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Source::GetButtonPressed() const
{
    n_error("VIRTUAL!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Source::GetButtonClicked() const
{
    n_error("VIRTUAL!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
Source::GetButtonDoubleClicked() const
{
    n_error("VIRTUAL!");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Source::AddEvent(Ptr<Event> newEvent)
{
    n_assert(newEvent != 0);
    this->events.Append(newEvent);
}

}; // namespace Input