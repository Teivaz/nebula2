//------------------------------------------------------------------------------
//  input/buttonsource.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/buttonsource.h"
#include "input/event.h"
#include "input/mapping.h"
#include "input/ninputevent.h"

namespace Input
{
ImplementRtti(Input::ButtonSource, Input::Source);
ImplementFactory(Input::ButtonSource);


//------------------------------------------------------------------------------
/**
*/
ButtonSource::ButtonSource() :
    buttonId(-1),
    buttonDown(false),
    buttonUp(false),
    buttonClicked(false),
    buttonDoubleClicked(false),
    resetState(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ButtonSource::~ButtonSource()
{
    // disconnect all events!
    while (this->buttonDownEvents.Size() > 0)
    {
        this->buttonDownEvents[0]->Disconnect();
        this->buttonDownEvents.Erase(0);
    }
    while (this->buttonUpEvents.Size() > 0)
    {
        this->buttonUpEvents[0]->Disconnect();
        this->buttonUpEvents.Erase(0);
    }
    while (this->buttonClickedEvents.Size() > 0)
    {
        this->buttonClickedEvents[0]->Disconnect();
        this->buttonClickedEvents.Erase(0);
    }
    while (this->buttonDoubleClickedEvents.Size() > 0)
    {
        this->buttonDoubleClickedEvents[0]->Disconnect();
        this->buttonDoubleClickedEvents.Erase(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::BeginFrame()
{
    // call parent
    Source::BeginFrame();

    // decrement up/down counter
    while (this->buttonDownEvents.Size() > 0 && this->buttonUpEvents.Size() > 0)
    {
        // decrement up/down pair together, so only completed clicks will be removed
        this->buttonDownEvents[0]->Disconnect();
        this->buttonDownEvents.Erase(0);

        this->buttonUpEvents[0]->Disconnect();
        this->buttonUpEvents.Erase(0);
    }

    // kill any thing greater than 1 (a button can't be down/up more than one)
    int numButtonDownEvents = n_min(this->buttonDownEvents.Size(), 1);
    while (this->buttonDownEvents.Size() > numButtonDownEvents)
    {
        this->buttonDownEvents[0]->Disconnect();
        this->buttonDownEvents.Erase(0);
    }

    int numButtonUpEvents = n_min(this->buttonUpEvents.Size(), 1);
    while (this->buttonUpEvents.Size() > numButtonUpEvents)
    {
        this->buttonUpEvents[0]->Disconnect();
        this->buttonUpEvents.Erase(0);
    }

    // clear all click events
    while (this->buttonClickedEvents.Size() > 0)
    {
        this->buttonClickedEvents[0]->Disconnect();
        this->buttonClickedEvents.Erase(0);
    }
    while (this->buttonDoubleClickedEvents.Size() > 0)
    {
        this->buttonDoubleClickedEvents[0]->Disconnect();
        this->buttonDoubleClickedEvents.Erase(0);
    }

    // clear flags
    this->buttonDown = false;
    this->buttonUp = false;
    this->buttonClicked = false;
    this->buttonDoubleClicked = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::Consume(nInputEvent* rawEvent)
{
    n_assert(rawEvent);

    if (rawEvent->GetDeviceId() == this->GetDeviceId())
    {
        if (this->GetButtonId() == rawEvent->GetButton())
        {
            vector2 mousePosition;
            Event::Button button = Event::UndefinedButton;
            bool hasMousePosition = false;
            if (this->GetDeviceId() == N_INPUT_MOUSE(0))
            {
                // a event from win proc, inclusive mouse position
                mousePosition.set(rawEvent->GetRelXPos(), rawEvent->GetRelYPos());
                hasMousePosition = true;

                if (rawEvent->GetButton() == 0)
                {
                    button = Event::LeftMouseButton;
                }
                else if (rawEvent->GetButton() == 1)
                {
                    button = Event::RightMouseButton;
                }
                else if (rawEvent->GetButton() == 2)
                {
                    button = Event::MiddleMouseButton;
                }
            }

            if (rawEvent->GetType() == N_INPUT_BUTTON_UP)
            {
                this->ButtonUp(mousePosition, hasMousePosition, button);
            }
            else if (rawEvent->GetType() == N_INPUT_BUTTON_DOWN)
            {
                this->ButtonDown(mousePosition, hasMousePosition, button);
            }
            else if (rawEvent->GetType() == N_INPUT_BUTTON_DBLCLCK)
            {
                this->ButtonDoubleClicked(mousePosition, hasMousePosition, button);
            }
        }
    }

    // listen to lose/obtain focus msgs
    if (rawEvent->GetType() == N_INPUT_LOSE_FOCUS || rawEvent->GetType() == N_INPUT_OBTAIN_FOCUS)
    {
        while(this->buttonDownEvents.Size() > this->buttonUpEvents.Size())
        {
            // create button up events until the button is relased
            this->ButtonUp();
        }
    }

    // call parent
    Source::Consume(rawEvent);
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::EndFrame()
{
    // emit one pressed event on each frame
    if (this->GetButtonPressed())
    {
        // create new button event
        Ptr<Event> event = Event::Create();
        event->SetType(Event::ButtonPressed);
        event->SetButton(Event::UndefinedButton);

        // send the event
        this->AddEvent(event);
    }

    // call parent
    Source::EndFrame();
}

//------------------------------------------------------------------------------
/**
*/
bool
ButtonSource::GetButtonDown() const
{
    return this->buttonDown;
}

//------------------------------------------------------------------------------
/**
*/
bool
ButtonSource::GetButtonUp() const
{
    return this->buttonUp;
}

//------------------------------------------------------------------------------
/**
*/
bool
ButtonSource::GetButtonPressed() const
{
    if (this->buttonDownEvents.Size() > this->buttonUpEvents.Size())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ButtonSource::GetButtonClicked() const
{
    return this->buttonClicked;
}

//------------------------------------------------------------------------------
/**
*/
bool
ButtonSource::GetButtonDoubleClicked() const
{
    return this->buttonDoubleClicked;
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::ButtonUp(const vector2& mousePosition, bool hasMousePosition, Event::Button button)
{
    // create new button up event
    Ptr<Event> event = Event::Create();
    event->SetType(Event::ButtonUp);
    event->SetButton(button);
    if (hasMousePosition)
    {
        event->SetMousePosition(mousePosition);
    }

    // send the up event
    this->AddEvent(event);

    // store the up event
    this->buttonUpEvents.Append(event);

    // if there is now a equal count of down and up events, emit a button clicked
    if (this->buttonDownEvents.Size() == this->buttonUpEvents.Size())
    {
        Ptr<Event> clickEvent = this->ButtonClicked(mousePosition, hasMousePosition, button);

        // connect the up event to the clicked event (!! this create a circular PTR reference !! to release this use Disconnect on the event)
        event->Connect(clickEvent);
    }
    else if (this->buttonDoubleClickedEvents.Size() > 0 && this->buttonDoubleClickedEvents.Size() >= this->buttonUpEvents.Size())
    {
        // ??? Don't know if this is really true but is seem like windows is doing so
        // connect the up event to the last dbl click event
        event->Connect(this->buttonDoubleClickedEvents[this->buttonDoubleClickedEvents.Size() -1]);
    }

    // set flag
    this->buttonUp = true;

}

//------------------------------------------------------------------------------
/**
*/
Event*
ButtonSource::ButtonDown(const vector2& mousePosition, bool hasMousePosition, Event::Button button)
{
    // create new button event
    Ptr<Event> event = Event::Create();
    event->SetType(Event::ButtonDown);
    event->SetButton(button);
    if (hasMousePosition)
    {
        event->SetMousePosition(mousePosition);
    }

    // send the event
    this->AddEvent(event);

    // store the event
    this->buttonDownEvents.Append(event);

    // the button is now down, so the upcounter must be downCount - 1 (this fixes missing up events)
    n_assert(this->buttonDownEvents.Size() >= 1);
    int numUpEvents = this->buttonDownEvents.Size() - 1;
    while (this->buttonUpEvents.Size() > numUpEvents)
    {
        this->buttonUpEvents[0]->Disconnect();
        this->buttonUpEvents.Erase(0);
    }

    // set flag
    this->buttonDown = true;

    return event;
}

//------------------------------------------------------------------------------
/**
*/
Event*
ButtonSource::ButtonClicked(const vector2& mousePosition, bool hasMousePosition, Event::Button button)
{
    // create new button event
    Ptr<Event> event = Event::Create();
    event->SetType(Event::ButtonClicked);
    event->SetButton(button);
    if (hasMousePosition)
    {
        event->SetMousePosition(mousePosition);
    }

    // connect to the up and down events
    if (this->buttonUpEvents.Size() >= 1)
    {
        event->Connect(this->buttonUpEvents[this->buttonUpEvents.Size() - 1]);
    }
    if (this->buttonDownEvents.Size() >= 1)
    {
        event->Connect(this->buttonDownEvents[this->buttonDownEvents.Size() - 1]);
    }

    // send event
    this->AddEvent(event);

    // remember event
    this->buttonClickedEvents.Append(event);

    // set flag
    this->buttonClicked = true;

    return event;
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::ButtonDoubleClicked(const vector2& mousePosition, bool hasMousePosition, Event::Button button)
{
    // create new button event
    Ptr<Event> event = Event::Create();
    event->SetType(Event::ButtonDoubleClicked);
    event->SetButton(button);
    if (hasMousePosition)
    {
        event->SetMousePosition(mousePosition);
    }

    // create a button down (windows does not send the 2nd down, if it is a dbl click)
    Ptr<Event> buttonDown = this->ButtonDown(mousePosition, hasMousePosition, button);
    buttonDown->Connect(event);

    // send event
    this->AddEvent(event);

    // remember event
    this->buttonDoubleClickedEvents.Append(event);

    // set flag
    this->buttonDoubleClicked = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ButtonSource::SetButtonId(int id)
{
    this->buttonId = id;
}

//------------------------------------------------------------------------------
/**
*/
int
ButtonSource::GetButtonId() const
{
    return this->buttonId;
}

}; // namespace Input