#ifndef PROPERTIES_ENVIRONMENTGRAPHICSPROPERTY_H
#define PROPERTIES_ENVIRONMENTGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::EnvironmentGraphicsProperty

    This is a specialized graphics property which handles all the static
    environment graphics. Probably 90% of all objects in a level will
    just be static environment objects, so it makes sense to create
    an optimized class just for that.

    NOTE: usually you don't need to care about this class. The level loader
    will automatically put all static environment objects into a single
    game entity which has a StaticGraphicsProperty attached.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace Properties
{
class EnvironmentGraphicsProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(EnvironmentGraphicsProperty);

public:
    /// constructor
    EnvironmentGraphicsProperty();
    /// destructor
    virtual ~EnvironmentGraphicsProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// add a graphics entity to the property, must have resource name and position
    void AddGraphicsEntity(Graphics::Entity* gfxEntity);
    /// attach several graphics entities to the property
    void AddGraphicsEntities(const nArray<Ptr<Graphics::Entity>>& gfxEntities);

private:
    nArray<nString> resNames;
    nArray<Ptr<Graphics::Entity> > graphicsEntities;
};

RegisterFactory(EnvironmentGraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
void
EnvironmentGraphicsProperty::AddGraphicsEntity(Graphics::Entity* gfxEntity)
{
    n_assert(gfxEntity);
    this->graphicsEntities.Append(gfxEntity);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
EnvironmentGraphicsProperty::AddGraphicsEntities(const nArray<Ptr<Graphics::Entity>>& gfxEntities)
{
    this->graphicsEntities.AppendArray(gfxEntities);
}

} // namespace Properties
//------------------------------------------------------------------------------
#endif
