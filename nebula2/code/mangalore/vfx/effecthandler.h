#ifndef VFX_EFFECTHANDLER_H
#define VFX_EFFECTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class VFX::EffectHandler

    Handles VFX subsystem related messages:

    PlayVisualEffect
    PlayShakeEffect
    
    (C) 2003 RadonLabs GmbH
*/
#include "message/port.h"

//------------------------------------------------------------------------------
namespace VFX
{
class EffectHandler : public Message::Port
{
    DeclareRtti;
	DeclareFactory(EffectHandler);

public:
    /// constructor
    EffectHandler();
    /// destructor
    virtual ~EffectHandler();
    /// return true if the port accepts a message id
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);
};

RegisterFactory(EffectHandler);

}; // namespace VFX

//------------------------------------------------------------------------------
#endif