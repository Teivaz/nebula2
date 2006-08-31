#ifndef PROPERTIES_SIMPLEGRAPHICSPROPERTY_H
#define PROPERTIES_SIMPLEGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::SimpleGraphicsProperty

    This is a simple graphics property which adds visibility to a game
    entity.

    (C) 2006 Radon Labs GmbH
*/
#include "properties/abstractgraphicsproperty.h"

//------------------------------------------------------------------------------
namespace Graphics
{
    class Entity;
};

namespace Properties
{
class SimpleGraphicsProperty : public AbstractGraphicsProperty
{
    DeclareRtti;
	DeclareFactory(SimpleGraphicsProperty);

public:
    /// constructor
    SimpleGraphicsProperty();
    /// destructor
    virtual ~SimpleGraphicsProperty();

    /// get the graphics entity
    Graphics::Entity* GetGraphicsEntity() const;

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// return true if message is accepted
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

    /// override to provide a other position than the entity for the graphics (default is Attr::Transform)
    virtual matrix44 GetGraphicsTransform();

protected:
    /// setup graphics entity
    virtual void SetupGraphicsEntity();
    /// cleanup graphics entity
    virtual void CleanupGraphicsEntity();
    /// is graphics loaded
    bool IsGraphicsActive() const;

    Ptr<Graphics::Entity> graphicsEntity;
};

RegisterFactory(SimpleGraphicsProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
