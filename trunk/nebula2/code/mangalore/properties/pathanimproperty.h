#ifndef PROPERTIES_PATHANIMPROPERTY_H
#define PROPERTIES_PATHANIMPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::PathAnimProperty

    Attach this property to an entity to move the entity along an
    animation path. Take care that the property won't collide
    with other properties which influence an entity's position.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "attr/attributes.h"
#include "anim2/nanimation.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(AnimPath);        // filename of animation file
    DeclareBool(AnimRelative);      // animate absolute position or from current position
    DeclareBool(AnimLoop);          // loop or clamp animations
    DeclareBool(AnimPlaying);       // true if animation currently playing
};

//------------------------------------------------------------------------------
namespace Properties
{
class PathAnimProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(PathAnimProperty);

public:
    /// constructor
    PathAnimProperty();
    /// destructor
    virtual ~PathAnimProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called before movement happens
    virtual void OnMoveBefore();
    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

private:
    /// start playback
    void Play();
    /// stop playback
    void Stop();
    /// rewind
    void Rewind();
    /// update game entity by animation
    void UpdateAnimation();

    nRef<nAnimation> refAnimation;
    matrix44 initialMatrix;
    nTime animTime;
};

RegisterFactory(PathAnimProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
