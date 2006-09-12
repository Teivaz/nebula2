#ifndef PROPERTIES_TRANSFORMABLEPROPERTY_H
#define PROPERTIES_TRANSFORMABLEPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::TransformableProperty

    Entities with this property can be transformed.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class TransformableProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(TransformableProperty);

public:
    /// constructor
    TransformableProperty();
    /// destructor
    virtual ~TransformableProperty();

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();

    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);
};

RegisterFactory(TransformableProperty);

} // namespace Properties
//------------------------------------------------------------------------------
#endif
