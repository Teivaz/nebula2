#ifndef UTIL_PHYSICSGFXUTIL_H
#define UTIL_PHYSICSGFXUTIL_H
//------------------------------------------------------------------------------
/**
    @class Util::PhysicsGfxUtil

    Helper class which knows how to connect complex physics composites to
    graphics entities.

    (C) 2005 Radon Labs GmbH
*/
#include "physics/entity.h"
#include "physics/ragdoll.h"
#include "graphics/entity.h"
#include "graphics/charentity.h"

//------------------------------------------------------------------------------
namespace Util
{
class PhysicsGfxUtil
{
public:
    /// create graphics entities for a physics entity
    static nArray<Ptr<Graphics::Entity> > CreateGraphics(Physics::Entity* physicsEntity);
    /// setup graphics entities for a physics entity
    static bool SetupGraphics(const nString& resourceName, Physics::Entity* physicsEntity, const nArray<Ptr<Graphics::Entity> >& graphicsEntities);
    /// update graphics transforms from physics entity
    static bool UpdateGraphicsTransforms(Physics::Entity* physicsEntity, const nArray<Ptr<Graphics::Entity> >& graphicsEntities);

private:
    /// create the right graphics entity(s) for a physics composite object
    static nArray<Ptr<Graphics::Entity> > CreateCompositeGraphics(Physics::Composite* composite);
    /// setup graphics entities for a physics composite object
    static bool SetupCompositeGraphics(const nString& resourceName, Physics::Composite* composite, const nArray<Ptr<Graphics::Entity> >& graphicsEntities);
    /// transfer physics composite position(s) into graphics entity(s)
    static bool TransferCompositeTransforms(Physics::Composite* composite, const nArray<Ptr<Graphics::Entity> >& graphicsEntities);
    /// create the right graphics entity for a physics ragdoll object
    static nArray<Ptr<Graphics::Entity> > CreateRagdollGraphics(Physics::Ragdoll* ragdoll);
    /// setup graphics entity for physics ragdoll object
    static bool SetupRagdollGraphics(const nString& resourceName, Physics::Entity* physicsEntity, Graphics::CharEntity* graphicsEntity);
    /// transfer ragdoll joint positions into graphics entity
    static bool TransferRagdollTransforms(Physics::Entity* physicsEntity, Graphics::CharEntity* graphicsEntity);
};

} // namespace Util
//------------------------------------------------------------------------------
#endif
