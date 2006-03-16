#ifndef PROPERTIES_ACTORANIMATIONPROPERTY_H
#define PROPERTIES_ACTORANIMATIONPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::ActorAnimationProperty

    Control the animations of a actor.
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class ActorAnimationProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(ActorAnimationProperty);

public:
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup idle as default
    virtual void OnActivate();
    /// listen to messages that may result to animation switch
    virtual bool Accepts(Message::Msg* msg);
    /// handle messages that may result to animation switch
    virtual void HandleMessage(Message::Msg* msg);

protected:
    /// create a animation switch msg
    void RequestAnimation(const nString& baseAnimation, const nString& overlayAnimation, float baseAnimTimeOffset) const;
};

RegisterFactory(ActorAnimationProperty);

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
