#ifndef PROPERTIES_ENVIRONMENTCOLLIDEPROPERTY_H
#define PROPERTIES_ENVIRONMENTCOLLIDEPROPERTY_H
//------------------------------------------------------------------------------
/**
    This property adds pieces of static collide geometry to the game world.
    It is very similar to the class EnvironmentGraphicsProperty, but instead
    of graphics it handles collision. All static collide geometry in a level
    will usually be added to one EnvironmentCollideProperty, which in
    turn lives in a single game entity which represent the environment
    graphics and collission. That way the game entity pool isn't flooded
    with hundreds of game entities which would end up doing nothing because
    they just represent static geometry. Instead, everything static about
    the level is put into a single entity.
    
    NOTE: usually you don't need to care about this class, it's used
    by the level loader which automatically collects all environment objects
    into a single game entity.

    (C) 2005 Radon Labs GmbH
*/    
#include "game/property.h"
#include "physics/shape.h"

//------------------------------------------------------------------------------
namespace Properties
{
class EnvironmentCollideProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(EnvironmentCollideProperty);

public:
    /// constructor
    EnvironmentCollideProperty();
    /// destructor
    virtual ~EnvironmentCollideProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// add a collide shape object
    void AddShape(Physics::Shape* shape);

private:
    nArray<Ptr<Physics::Shape> > collideShapes;
};

RegisterFactory(EnvironmentCollideProperty);

//------------------------------------------------------------------------------
/**
*/
inline
void
EnvironmentCollideProperty::AddShape(Physics::Shape* shape)
{
    n_assert(shape);
    this->collideShapes.Append(shape);
}

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
    