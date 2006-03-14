#ifndef PHYSICS_AREAIMPULSE_H
#define PHYSICS_AREAIMPULSE_H
//------------------------------------------------------------------------------
/**
    @class Physics::AreaImpulse

    General base class for area impulses. An area impulse applies an impulse
    to all objects within a given area volume. Subclasses implement specific
    volumes and behaviours. Most useful for explosions and similar stuff.
    
    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Physics
{
class AreaImpulse : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(AreaImpulse);

public:
    /// constructor
    AreaImpulse();
    /// destructor
    virtual ~AreaImpulse();
    /// apply the impulse to the world, override this method in a subclass
    virtual void Apply();
};

RegisterFactory(AreaImpulse);

}; // namespace Physics

//------------------------------------------------------------------------------
#endif