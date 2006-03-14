#ifndef AUDIO_SOUNDFXHANDLER_H
#define AUDIO_SOUNDFXHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Audio::SoundFxHandler

    The global sound fx handler implements the following messages:

    PlaySound

    (C) 2003 RadonLabs GmbH
*/
#include "message/port.h"
#include "msg/playsound.h"

//------------------------------------------------------------------------------
namespace Audio
{
class SoundFxHandler : public Message::Port
{
    DeclareRtti;
	DeclareFactory(SoundFxHandler);

public:
    /// constructor
    SoundFxHandler();
    /// destructor
    virtual ~SoundFxHandler();
    /// return true if the port accepts a message id
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

private:
    /// handle a SoundFxPlay message
    void HandlePlaySound(Message::PlaySound* msg);
};

RegisterFactory(SoundFxHandler);

}; // namespace Audio

//------------------------------------------------------------------------------
#endif

