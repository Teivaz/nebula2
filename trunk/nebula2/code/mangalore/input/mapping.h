#ifndef INPUT_MAPPING_H
#define INPUT_MAPPING_H
//------------------------------------------------------------------------------
/**
    @class Input::Mapping

    Defines what raw input device-channels fed's what input sources.

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "input/chain.h"

class nInputEvent;

namespace Input
{
class Source;
class Event;

class Mapping : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Mapping);

public:
    /// constructor
    Mapping();
    /// destructor
    virtual ~Mapping();

    /// begin input frame
    void BeginFrame();
    /// consume raw events
    void Consume();
    /// emit input events
    void Emit();
    /// end frame
    void EndFrame();

    /// set mapping name
    void SetName(const nString& name);
    /// get mapping name
    const nString& GetName() const;

    /// is a input source attached to the mapping?
    bool HasInputSource() const;
    /// set the mapping input source (a appropriate converter for the raw events)
    void SetInputSource(Ptr<Source> source);
    /// get the input source of this mapping (only valid if there is a valid input device set)
    Source* GetInputSource() const;

    // ---- attach/remove input sinks to the chain of this mapping
    /// attach a input sink, the priority defines the order in the chain
    void AttachInputSink(int priority, Message::Port* sink);
    /// remove a input sink (from all priorities)
    void RemoveInputSink(Message::Port* sink);
    /// remove the input sink from the given priority
    void RemoveInputSink(int priority, Message::Port* sink);

    /// send a array of events into the input chain
    void SendEvents(const nArray<Ptr<Event> >& events);

private:
    /// the embedded input chain for this mapping
    Ptr<Chain> inputChain;

    /// the input source that is able to convert the raw events of the current set device
    Ptr<Source> inputSource;

    /// the name of the mapping
    nString name;
};

} // namespace Input
//------------------------------------------------------------------------------
#endif
