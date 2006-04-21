#ifndef PROPERTIES_EFFECTPROPERTY_H
#define PROPERTIES_EFFECTPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::EffectProperty

    An effect property adds the ability to emit particles to a game entity.

    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class EffectProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(EffectProperty);

public:

};

RegisterFactory(EffectProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif
