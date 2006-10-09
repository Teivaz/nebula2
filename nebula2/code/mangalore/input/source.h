#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H
//------------------------------------------------------------------------------
/**
    @class Input::Source

    Emit Input:Events, because of consumed raw device events.

    (C) 2006 RadonLabs GmbH
*/
#include "foundation/refcounted.h"

class nInputEvent;

namespace Input
{
class Chain;
class Mapping;
class Event;

class Source : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Source);

public:
    /// constructor
    Source();

    /// begin frame (reset states as needed)
    virtual void BeginFrame();
    /// consume raw event
    virtual void Consume(nInputEvent* rawEvent);
    /// end frame (combine states as needed)
    virtual void EndFrame();

    /// emit the input of the last frame via mapping
    virtual void Emit(Mapping* mapping);

    /// notify the source how many sink are attached (used to disable the key-buttons, when a characters mapping is active)
    virtual void NotifyNumSinks(int num);

    /// get device id this source listens to
    int GetDeviceId() const;
    /// set device id this source listens to
    void SetDeviceId(int deviceId);

    /// ---- to be able to poll common button and slider states some virtual functions ----
    /// get axis value (formerly known as GetSlider())
    virtual float GetAxisValue() const;

    /// has the button gone down in the last frame?
    virtual bool GetButtonDown() const;
    /// has the button gone up in the last frame?
    virtual bool GetButtonUp() const;

    /// is the button now pressed?
    virtual bool GetButtonPressed() const;
    /// was there a button click (up and down) in the last frame?
    virtual bool GetButtonClicked() const;
    /// was there a button double click in the last frame? (only the windows mouse dbl clicks are handled)
    virtual bool GetButtonDoubleClicked() const;

protected:
    /// add a event that should be emitted this frame
    void AddEvent(Ptr<Event> newEvent);

private:
    int deviceId;

    /// the events that should be emitted this frame
    nArray<Ptr<Event> > events;
};

} // namespace Input
//------------------------------------------------------------------------------
#endif