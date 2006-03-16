#ifndef PROPERTIES_PHYSICSPROPERTY_H
#define PROPERTIES_PHYSICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::PhysicsProperty

    A physics property adds basic physical behaviour to a game entity. 
    The default behaviour is that of a passive physics object which will
    just passively roll and bounce around. Implement more advanced behaviour
    in subclasses.

    The physics property maintains the attributes:

    Attr::Transform
    Attr::Velocity
  
    (C) 2005 Radon Labs GmbH
*/
#include "properties/abstractphysicsproperty.h"
#include "physics/entity.h"
#include "attr/attributes.h"

//------------------------------------------------------------------------------
namespace Properties
{
class PhysicsProperty : public AbstractPhysicsProperty
{
    DeclareRtti;
	DeclareFactory(PhysicsProperty);

public:
    /// constructor
    PhysicsProperty();
    /// destructor
    virtual ~PhysicsProperty();

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();

    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called after movement has happened
    virtual void OnMoveAfter();

    /// get a pointer to the physics entity
    virtual Physics::Entity* GetPhysicsEntity() const;

    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

protected:
    /// enable and activate the physics, overload in subclass
    virtual void EnablePhysics();
    /// disable and cleanup the physics, overload in subclass
    virtual void DisablePhysics();

private:
    Ptr<Physics::Entity> physicsEntity;
};

RegisterFactory(PhysicsProperty);

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
