//------------------------------------------------------------------------------
//  audio/soundfxhandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/soundfxhandler.h"
#include "audio/server.h"

namespace Audio
{
ImplementRtti(Audio::SoundFxHandler, Message::Port);
ImplementFactory(Audio::SoundFxHandler);

//------------------------------------------------------------------------------
/**
*/
SoundFxHandler::SoundFxHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SoundFxHandler::~SoundFxHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
SoundFxHandler::Accepts(Message::Msg* msg)
{
    return msg->CheckId(Message::PlaySound::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
SoundFxHandler::HandleMessage(Message::Msg* msg)
{
    if (msg->CheckId(Message::PlaySound::Id))
    {
        Message::PlaySound* playSoundMsg = (Message::PlaySound*) msg;
        Audio::Server::Instance()->PlaySoundEffect(playSoundMsg->GetName(), 
                                                   playSoundMsg->GetPosition(), 
                                                   playSoundMsg->GetVelocity(), 
                                                   playSoundMsg->GetVolume());
    }
}

} // namespace Audio