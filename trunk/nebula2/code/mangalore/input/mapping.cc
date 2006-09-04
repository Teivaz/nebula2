//------------------------------------------------------------------------------
//  input/mapping.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/mapping.h"
#include "input/source.h"
#include "input/event.h"
#include "input/ninputserver.h"

namespace Input
{
ImplementRtti(Input::Mapping, Foundation::RefCounted);
ImplementFactory(Input::Mapping);

//------------------------------------------------------------------------------
/**
*/
Mapping::Mapping()
{
    // create embedded input chain
    n_assert(this->inputChain == 0);
    this->inputChain = Chain::Create();
}

//------------------------------------------------------------------------------
/**
*/
Mapping::~Mapping()
{
    // cleanup embedded input chain
    n_assert(this->inputChain != 0);
    this->inputChain = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::BeginFrame()
{
    // notify the chain that the frame begins
    if (this->inputChain != 0)
    {
        this->inputChain->BeginFrame();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::EndFrame()
{
    // notify the chain that the frame ends
    if (this->inputChain != 0)
    {
        this->inputChain->EndFrame();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::Consume()
{
    if (this->inputSource != 0)
    {
        this->inputSource->BeginFrame();

        nInputServer* inputServer = nInputServer::Instance();
        n_assert(inputServer);

        // get all raw input events from nebula of this frame
        nInputEvent* inputEvent = 0;
        for (inputEvent = inputServer->FirstEvent(); inputEvent != 0; inputEvent = inputServer->NextEvent(inputEvent))
        {
            n_assert(inputEvent != 0);
            this->inputSource->Consume(inputEvent);
        }

        this->inputSource->EndFrame();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::Emit()
{
    if (this->inputSource != 0)
    {
        this->inputSource->Emit(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::SendEvents(const nArray<Ptr<Event> >& events)
{
    // set the mapping
    for (int i = 0; i < events.Size(); i++)
    {
        events[i]->SetMapping(this);
    }

    // dispatch
    n_assert(this->inputChain != 0);
    this->inputChain->SendEvents(events);
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::SetName(const nString& n)
{
    n_assert(n.IsValid());
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
const nString&
Mapping::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::SetInputSource(Ptr<Source> source)
{
    n_assert(source != 0);
    this->inputSource = source;

    // notify the input source about the number of attached sinks
    this->inputSource->NotifyNumSinks(this->inputChain->GetNumSinks());
}

//------------------------------------------------------------------------------
/**
*/
Source*
Mapping::GetInputSource() const
{
    n_assert(this->inputSource != 0);
    return this->inputSource;
}

//------------------------------------------------------------------------------
/**
*/
bool
Mapping::HasInputSource() const
{
    return (this->inputSource != 0);
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::AttachInputSink(int priority, Message::Port* sink)
{
    n_assert(this->inputChain != 0);
    n_assert(sink);
    this->inputChain->AttachInputSink(priority, sink);

    // notify the input source about the number of attached sinks
    if (this->inputSource != 0)
    {
        this->inputSource->NotifyNumSinks(this->inputChain->GetNumSinks());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::RemoveInputSink(Message::Port* sink)
{
    n_assert(this->inputChain != 0);
    n_assert(sink);
    this->inputChain->RemoveInputSink(sink);

    // notify the input source about the number of attached sinks
    if (this->inputSource != 0)
    {
        this->inputSource->NotifyNumSinks(this->inputChain->GetNumSinks());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Mapping::RemoveInputSink(int priority, Message::Port* sink)
{
    n_assert(this->inputChain != 0);
    n_assert(sink);
    this->inputChain->RemoveInputSink(priority, sink);

    // notify the input source about the number of attached sinks
    if (this->inputSource != 0)
    {
        this->inputSource->NotifyNumSinks(this->inputChain->GetNumSinks());
    }
}

}; // namespace Input