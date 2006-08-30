#ifndef INPUT_BUTTONSOURCE_H
#define INPUT_BUTTONSOURCE_H
//------------------------------------------------------------------------------
/**
    @class Input::ButtonSource

    A special Input::Source, that emit button up and down events keep track of
    the current button state.

    (C) 2006 RadonLabs GmbH
*/
#include "input/source.h"
#include "input/event.h"

namespace Input
{

class ButtonSource : public Source
{
    DeclareRtti;
    DeclareFactory(ButtonSource);

public:
    /// constructor
    ButtonSource();
    /// destructor
    virtual ~ButtonSource();

    /// set button id this source listens to
    void SetButtonId(int buttonId);
    /// get button this source listens to
    int GetButtonId() const;

    /// begin frame (reset states as needed)
    virtual void BeginFrame();
    /// consume raw event
    virtual void Consume(nInputEvent* rawEvent);
    /// end frame (combine states as needed)
    virtual void EndFrame();

    /// has the button gone down in the last frame?
    virtual bool GetButtonDown() const;
    /// has the button gone up in the last frame?
    virtual bool GetButtonUp() const;

    /// is the button now pressed?
    virtual bool GetButtonPressed() const;
    /// was there a button click (up and down) in the last frame?
    virtual bool GetButtonClicked() const;
    /// was there a button double click (2 clicks in a short time) in the last frame?
    virtual bool GetButtonDoubleClicked() const;

protected:
    /// a button up was registered
    void ButtonUp(const vector2& mousePosition = vector2(0.f, 0.f), bool hasMousePosition = false, Event::Button button = Event::UndefinedButton);
    /// a button down was registered
    Event* ButtonDown(const vector2& mousePosition = vector2(0.f, 0.f), bool hasMousePosition = false, Event::Button button = Event::UndefinedButton);
    /// a button click was registered
    Event* ButtonClicked(const vector2& mousePosition = vector2(0.f, 0.f), bool hasMousePosition = false, Event::Button button = Event::UndefinedButton);
    /// a button double click was registered
    void ButtonDoubleClicked(const vector2& mousePosition = vector2(0.f, 0.f), bool hasMousePosition = false, Event::Button button = Event::UndefinedButton);

private:
    int buttonId;

    nArray<Ptr<Event> > buttonDownEvents;
    nArray<Ptr<Event> > buttonUpEvents;
    nArray<Ptr<Event> > buttonClickedEvents;
    nArray<Ptr<Event> > buttonDoubleClickedEvents;

    bool buttonDown;
    bool buttonUp;
    bool buttonClicked;
    bool buttonDoubleClicked;

    bool resetState;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif