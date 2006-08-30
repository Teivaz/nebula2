#ifndef INPUT_KEYSOURCE_H
#define INPUT_KEYSOURCE_H
//------------------------------------------------------------------------------
/**
    @class Input::KeySource

    A special source that controlls the character input. All KeySources share
    a flag that is set if a textinput mapping has a sink attached (one grabs
    the raw characters). If this is the case the emit of button events for all
    keys is disabled!

    (C) 2006 RadonLabs GmbH
*/
#include "input/buttonsource.h"

class nInputEvent;

namespace Input
{

class KeySource : public ButtonSource
{
    DeclareRtti;
    DeclareFactory(KeySource);

public:
    /// constructor
    KeySource();
    /// destructor
    virtual ~KeySource();

    /// consume raw events
    virtual void Consume(nInputEvent* rawEvent);

    /// set key id
    void SetKeyId(int id);
    /// get key id
    int GetKeyId() const;

    /// mark this key source as the character source, so if a sink is attach to this source mapping
    /// all other key sources (those for the key-buttons where muted)
    void SetIsCharacterSource(bool isCharacterSource);
    /// is this input source a character input source
    bool GetIsCharacterSource() const;

    /// disable the key-button event generation while one character input mapping is active
    virtual void NotifyNumSinks(int num);

private:
    int keyId;
    static int muteKeyButtons;
    bool unmuteOnDie;
    bool isCharacterInputSource;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif