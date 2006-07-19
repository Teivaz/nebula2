#ifndef VFX_SENDMESSAGEEFFECT_H
#define VFX_SENDMESSAGEEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::SendMessageEffect
    
    Fires any number of stored messages at the time when the sound effect 
    is played.
    
    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"
#include "message/msg.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
namespace VFX
{
class SendMessageEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(SendMessageEffect);
public:
    /// constructor
    SendMessageEffect();
    /// destructor
    virtual ~SendMessageEffect();
    /// add a message to be sent
    void AddMessage(Message::Msg* msg);
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart();

private:
    nArray<Ptr<Message::Msg> > messages;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
SendMessageEffect::AddMessage(Message::Msg* msg)
{
    n_assert(0 != msg);
    this->messages.Append(msg);
}

}; // namespace VFX
//------------------------------------------------------------------------------
#endif
