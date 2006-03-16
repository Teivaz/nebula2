#ifndef PROPERTIES_ABSTRACTGRAPHICSPROPERTY_H
#define PROPERTIES_ABSTRACTGRAPHICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::AbstractGraphicsProperty

    Abstract Graphics Property.

    Baseclass for all Graphics Propertys.
    
    (C) 2006 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class AbstractGraphicsProperty : public Game::Property
{
    DeclareRtti;
    DeclareFactory(AbstractGraphicsProperty);

public:
    /// constructor
    AbstractGraphicsProperty();
    /// destructor
    virtual ~AbstractGraphicsProperty();

    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    
};

RegisterFactory(AbstractGraphicsProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif
