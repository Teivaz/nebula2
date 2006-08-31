//------------------------------------------------------------------------------
//  vfx/effecthandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "vfx/effecthandler.h"
#include "vfx/server.h"
#include "msg/playvisualeffect.h"
#include "msg/playshakeeffect.h"

namespace VFX
{
ImplementRtti(VFX::EffectHandler, Message::Port);
ImplementFactory(VFX::EffectHandler);

//------------------------------------------------------------------------------
/**
*/
EffectHandler::EffectHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EffectHandler::~EffectHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
EffectHandler::Accepts(Message::Msg* msg)
{
    return msg->CheckId(Message::PlayVisualEffect::Id) ||
           msg->CheckId(Message::PlayShakeEffect::Id);
}

//------------------------------------------------------------------------------
/**
*/
void
EffectHandler::HandleMessage(Message::Msg* msg)
{
    Server* server = Server::Instance();
    if (msg->CheckId(Message::PlayVisualEffect::Id))
    {
        // play a visual effect
        Message::PlayVisualEffect* pveMsg = (Message::PlayVisualEffect*) msg;
        GraphicsEffect* effect = server->CreateGraphicsEffect(pveMsg->GetName(), pveMsg->GetTransform());
        if (0 != effect)
        {
            server->AttachEffect(effect);
        }
        else
        {
        #ifdef _DEBUG
            n_printf("EffectHandler::HandleMessage: effect '%s' not found! \n", pveMsg->GetName().Get());
        #endif // _DEBUG
        }
    }
    else if (msg->CheckId(Message::PlayShakeEffect::Id))
    {
        // play a shake effect
        Message::PlayShakeEffect* pseMsg = (Message::PlayShakeEffect*) msg;
        ShakeEffect* effect = server->CreateShakeEffect(pseMsg->GetTransform(),
                                  pseMsg->GetRange(),
                                  pseMsg->GetDuration(),
                                  pseMsg->GetIntensity());
        server->AttachEffect(effect);
    }
}

} // namespace VFX
