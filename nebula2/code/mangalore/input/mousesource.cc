//------------------------------------------------------------------------------
//  input/mousesource.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/mousesource.h"
#include "input/event.h"
#include "input/mapping.h"
#include "input/ninputevent.h"

namespace Input
{
ImplementRtti(Input::MouseSource, Input::Source);
ImplementFactory(Input::MouseSource);

//------------------------------------------------------------------------------
/**
*/
MouseSource::MouseSource() :
    relMousePosition(0.f, 0.f),
    absMousePositionX(0),
    absMousePositionY(0),
    mouseMoved(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MouseSource::BeginFrame()
{
    // call parent
    Source::BeginFrame();

    this->mouseMoved = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MouseSource::Consume(nInputEvent* rawEvent)
{
    n_assert(rawEvent);

    if (rawEvent->GetType() == N_INPUT_MOUSE_MOVE)
    {
        if (rawEvent->GetDeviceId() == this->GetDeviceId())
        {
            vector2 newRelPos(rawEvent->GetRelXPos(), rawEvent->GetRelYPos());
            int newAbsX = rawEvent->GetAbsXPos();
            int newAbsY = rawEvent->GetAbsYPos();
            if (!this->relMousePosition.isequal(newRelPos, 0.000001f) ||
                newAbsX != this->absMousePositionX ||
                newAbsY != this->absMousePositionY)
            {
                this->mouseMoved = true;

                this->relMousePosition = newRelPos;
                this->absMousePositionX = newAbsX;
                this->absMousePositionY = newAbsY;
            }
        }
    }

    // call parent
    Source::Consume(rawEvent);
}

//------------------------------------------------------------------------------
/**
*/
void
MouseSource::EndFrame()
{
    if (this->mouseMoved)
    {
        // create axis moved input event
        Ptr<Event> event = Event::Create();
        event->SetType(Event::MouseMoved);
        event->SetRelMousePosition(this->relMousePosition);
        event->SetAbsMousePosition(this->absMousePositionX, this->absMousePositionY);

        this->AddEvent(event);
    }

    // call parent
    Source::EndFrame();
}


} // namespace Input