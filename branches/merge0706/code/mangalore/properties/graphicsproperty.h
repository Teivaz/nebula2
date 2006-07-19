#ifndef PROPERTIES_GRAPHICSPROPERTY_H
#define PROPERTIES_GRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::GraphicsProperty

    This is the standard graphics property which adds visibility to a game
    entity.

    NOTE: There are cases where the graphics property may depend on a
    physics property (for complex physics entities which require several
    graphics entities to render themselves). Thus it is recommended that
    physics properties are attached before graphics properties.

    (C) 2005 Radon Labs GmbH
*/
#include "properties/abstractgraphicsproperty.h"

//------------------------------------------------------------------------------
namespace Graphics
{
    class Entity;
};

namespace Properties
{
class GraphicsProperty : public AbstractGraphicsProperty
{
    DeclareRtti;
	DeclareFactory(GraphicsProperty);

public:
    /// constructor
    GraphicsProperty();
    /// destructor
    virtual ~GraphicsProperty();

    /// get a reference to the array of graphics entities
    const nArray<Ptr<Graphics::Entity> >& GetGraphicsEntities() const;

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

protected:
    /// setup graphics entities
    virtual void SetupGraphicsEntities();
    /// update the graphics entity's transform
    virtual void UpdateTransform(const matrix44& m, bool setDirectly=false);

    nArray<Ptr<Graphics::Entity> > graphicsEntities;

private:
    // Shows or hides all attached graphics entities (Message::GfxSetVisible).
    void SetVisible(bool visible);
};

RegisterFactory(GraphicsProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
