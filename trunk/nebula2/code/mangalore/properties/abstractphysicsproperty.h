#ifndef PROPERTIES_ABSTRACTPHYSICSPROPERTY_H
#define PROPERTIES_ABSTRACTPHYSICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::AbstractPhysicsProperty

    Abstract Physics property, provides a enable and disable mechanism.
  
    (C) 2005 Radon Labs GmbH
*/
#include "properties/transformableproperty.h"

//------------------------------------------------------------------------------
namespace Physics
{
    class Entity;
};

//------------------------------------------------------------------------------
namespace Properties
{
class AbstractPhysicsProperty : public TransformableProperty
{
    DeclareRtti;
	DeclareFactory(AbstractPhysicsProperty);

public:
    /// constructor
    AbstractPhysicsProperty();
    /// destructor
    virtual ~AbstractPhysicsProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// enable/disable physics
    void SetEnabled(bool enabled);
    /// is physics enabled
    bool IsEnabled() const;

    /// get a pointer to the physics entity
    virtual Physics::Entity* GetPhysicsEntity() const;

protected:
    /// enable and activate the physics, overload in subclass
    virtual void EnablePhysics();
    /// disable and cleanup the physics, overload in subclass
    virtual void DisablePhysics();

private:
    bool enabled;
};

RegisterFactory(AbstractPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
AbstractPhysicsProperty::IsEnabled() const
{
    return this->enabled;    
}

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
