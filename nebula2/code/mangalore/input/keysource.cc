//------------------------------------------------------------------------------
//  input/keysource.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "input/keysource.h"
#include "input/event.h"
#include "input/ninputevent.h"

namespace Input
{
ImplementRtti(Input::KeySource, Input::ButtonSource);
ImplementFactory(Input::KeySource);

int KeySource::muteKeyButtons = 0;

//------------------------------------------------------------------------------
/**
*/
KeySource::KeySource() :
    isCharacterInputSource(false),
    unmuteOnDie(false),
    keyId(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
KeySource::~KeySource()
{
    if (this->unmuteOnDie)
    {
        n_assert(this->muteKeyButtons > 0);
        this->muteKeyButtons--;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
KeySource::Consume(nInputEvent* rawEvent)
{
    n_assert(rawEvent);

    if (rawEvent->GetDeviceId() == this->GetDeviceId())
    {
        if (rawEvent->GetType() == N_INPUT_KEY_CHAR)
        {
            // create character event
            Ptr<Event> event = Event::Create();
            event->SetType(Event::Character);
            event->SetCharacterCode(rawEvent->GetChar());

            this->AddEvent(event);
        }
        else
        {
            // handle key events or mute because some one grabs the characters
            if (!this->muteKeyButtons)
            {
                if (this->GetKeyId() == (int)rawEvent->GetKey())
                {
                    if (rawEvent->GetType() == N_INPUT_KEY_UP)
                    {
                        this->ButtonUp();
                    }
                    else if (rawEvent->GetType() == N_INPUT_KEY_DOWN)
                    {
                        // windows send's button down, while the key is pressed for each repeat of the char, filter this
                        if (!this->GetButtonPressed())
                        {
                            this->ButtonDown();
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
KeySource::NotifyNumSinks(int num)
{
    if (this->isCharacterInputSource)
    {
        // disable button events while someone grabs characters
        if (num > 0)
        {
            if (!this->unmuteOnDie)
            {
                this->muteKeyButtons++;
                this->unmuteOnDie = true;
            }
        }
        else
        {
            if (this->unmuteOnDie)
            {
                this->muteKeyButtons--;
                this->unmuteOnDie = false;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
KeySource::SetIsCharacterSource(bool is)
{
    this->isCharacterInputSource = is;
}

//------------------------------------------------------------------------------
/**
*/
bool
KeySource::GetIsCharacterSource() const
{
    return this->isCharacterInputSource;
}

//------------------------------------------------------------------------------
/**
*/
void
KeySource::SetKeyId(int id)
{
    this->keyId = id;
}

//------------------------------------------------------------------------------
/**
*/
int
KeySource::GetKeyId() const
{
    return this->keyId;
}

}; // namespace Input