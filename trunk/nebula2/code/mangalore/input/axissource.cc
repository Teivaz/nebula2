//------------------------------------------------------------------------------
//  input/axissource.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/axissource.h"
#include "input/event.h"
#include "input/mapping.h"
#include "input/ninputevent.h"

namespace Input
{
ImplementRtti(Input::AxisSource, Input::Source);
ImplementFactory(Input::AxisSource);

//------------------------------------------------------------------------------
/**
*/
AxisSource::AxisSource() :
    axisValue(0.f),
    axisMoved(false),
    axis(-1)
{
    // init axis filter
    this->axisFilter.SetSize(AxisFilterSize);
}

//------------------------------------------------------------------------------
/**
*/
void
AxisSource::BeginFrame()
{
    // call parent
    Source::BeginFrame();

    this->axisMoved = false;
    this->axisValue = 0.f;
}

//------------------------------------------------------------------------------
/**
*/
void
AxisSource::Consume(nInputEvent* rawEvent)
{
    n_assert(rawEvent);

    if(rawEvent->GetDeviceId() == this->GetDeviceId())
    {
        if (rawEvent->GetType() == N_INPUT_AXIS_MOVE)
        {
            if (this->GetAxis() == rawEvent->GetAxis())
            {
                if (this->axisValue != rawEvent->GetAxisValue())
                {
                    this->axisValue += rawEvent->GetAxisValue();
                    this->axisMoved = true;
                }
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
AxisSource::EndFrame()
{
    if (this->axisMoved)
    {
        // create axis moved input event
        Ptr<Event> event = Event::Create();
        event->SetType(Event::AxisMoved);

        // filter value
        this->axisFilter.PushValue(this->axisValue);

        // use filtered value
        event->SetAxisValue(this->axisFilter.GetFilteredValue());

        this->AddEvent(event);
    } else {
        this->axisFilter.PushValue(0.0f);
    }

    // call parent
    Source::EndFrame();
}

//------------------------------------------------------------------------------
/**
*/
int
AxisSource::GetAxis() const
{
    return this->axis;
}

//------------------------------------------------------------------------------
/**
*/
void
AxisSource::SetAxis(int a)
{
    this->axis = a;
}

//------------------------------------------------------------------------------
/**
*/
float
AxisSource::GetAxisValue() const
{
    return this->axisFilter.GetFilteredValue();
}

}; // namespace Input