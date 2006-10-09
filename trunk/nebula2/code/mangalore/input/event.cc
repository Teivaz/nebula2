//------------------------------------------------------------------------------
//  input/event.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/event.h"
#include "input/mapping.h"

namespace Input
{
ImplementRtti(Input::Event, Message::Msg);
ImplementFactory(Input::Event);
ImplementMsgId(Input::Event);

//------------------------------------------------------------------------------
/**
*/
Event::Event() :
    type(InvalidEventType),
    mousePosition(-1.f, -1.f),
    axisValue(0.f),
    characterCode(-1),
    handled(false),
    hasMousePosition(false),
    button(UndefinedButton),
    priority(0),
    mapping(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Event::~Event()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Event::Type
Event::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetType(Type t)
{
    n_assert(t >= 0 && t < NumEventType);
    this->type = t;
}

//------------------------------------------------------------------------------
/**
    be 3 levels recursive by default,
    needed to detect that up is handled because dbl click is handled
    up -> click -> down -> dblClick(handled)
*/
bool
Event::Handled(int recursive) const
{
    if (this->handled)
    {
        return true;
    }

    // check if one of the connected events already is handled
    if (recursive > 0)
    {
        for (int i = 0; i < this->connectedEvents.Size(); i++)
        {
            if (this->connectedEvents[i]->Handled(recursive - 1))
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetHandled()
{
    this->handled = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::Connect(Event* relatedEvent)
{
    n_assert(relatedEvent);
    this->connectedEvents.Append(relatedEvent);
}

//------------------------------------------------------------------------------
/**
*/
void
Event::Disconnect()
{
    this->connectedEvents.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetMapping(Mapping* m)
{
    n_assert(m);
    this->mapping = m;
}

//------------------------------------------------------------------------------
/**
*/
Mapping*
Event::GetMapping() const
{
    n_assert(this->mapping != 0);
    return this->mapping;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetMousePosition(const vector2& pos)
{
    this->mousePosition = pos;
    this->hasMousePosition = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Event::HasMousePosition() const
{
    return this->hasMousePosition;
}

//------------------------------------------------------------------------------
/**
*/
const vector2&
Event::GetMousePosition() const
{
    return this->mousePosition;
}

//------------------------------------------------------------------------------
/**
*/
float
Event::GetAxisValue() const
{
    return this->axisValue;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetAxisValue(float val)
{
    this->axisValue = val;
}

//------------------------------------------------------------------------------
/**
*/
int
Event::GetCharacterCode() const
{
    return this->characterCode;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetCharacterCode(int code)
{
    this->characterCode = code;
}

//------------------------------------------------------------------------------
/**
*/
int
Event::GetAbsMousePositionX() const
{
    return this->absMousePositionX;
}

//------------------------------------------------------------------------------
/**
*/
int
Event::GetAbsMousePositionY() const
{
    return this->absMousePositionY;
}

//------------------------------------------------------------------------------
/**
*/
const vector2&
Event::GetRelMousePosition() const
{
    return this->relMousePosition;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetAbsMousePosition(int x, int y)
{
    this->absMousePositionX = x;
    this->absMousePositionY = y;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetRelMousePosition(const vector2& pos)
{
    this->relMousePosition = pos;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetButton(Button b)
{
    n_assert(b >= 0 && b < NumButtonTypes);
    this->button = b;
}

//------------------------------------------------------------------------------
/**
*/
Event::Button
Event::GetButton() const
{
    return this->button;
}

//------------------------------------------------------------------------------
/**
*/
void
Event::SetPriority(int p)
{
    this->priority = p;
}

//------------------------------------------------------------------------------
/**
*/
int
Event::GetPriority() const
{
    return this->priority;
}

} // namespace Input