#ifndef PROPERTIES_AUDIOPROPERTY_H
#define PROPERTIES_AUDIOPROPERTY_H
//------------------------------------------------------------------------------
/**
    An audio property adds the ability to emit sounds to a game entity.
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"

//------------------------------------------------------------------------------
namespace Properties
{
class AudioProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(AudioProperty);

public:

};

RegisterFactory(AudioProperty);

}; // namespace Property
//------------------------------------------------------------------------------
#endif