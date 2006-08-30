//------------------------------------------------------------------------------
//  util/physicsgfxutil.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "util/physicsgfxutil.h"
#include "graphics/server.h"
#include "scene/ntransformnode.h"
#include "physics/composite.h"
#include "physics/ragdoll.h"
#include "graphics/charentity.h"
#include "foundation/factory.h"

namespace Util
{
//------------------------------------------------------------------------------
/**
    This will create one or several graphics entities which can visualize
    the physics entity.
*/
nArray<Ptr<Graphics::Entity> >
PhysicsGfxUtil::CreateGraphics(Physics::Entity* physicsEntity)
{
    n_assert(physicsEntity);
    Physics::Composite* composite = physicsEntity->GetComposite();
    if (composite->IsA(Physics::Ragdoll::RTTI))
    {
        return CreateRagdollGraphics((Physics::Ragdoll*) composite);
    }
    else
    {
        return CreateCompositeGraphics(composite);
    }
}

//------------------------------------------------------------------------------
/**
    Setup graphics entities created by CreateGraphics().
*/
bool
PhysicsGfxUtil::SetupGraphics(const nString& resourceName, Physics::Entity* physicsEntity, const nArray<Ptr<Graphics::Entity> >& graphicsEntities)
{
    n_assert(physicsEntity);
    Physics::Composite* composite = physicsEntity->GetComposite();

    // FIXME: handle ragdolls based on composite's type
    if (composite->IsA(Physics::Ragdoll::RTTI))
    {
        return SetupRagdollGraphics(resourceName, physicsEntity, (Graphics::CharEntity*) graphicsEntities[0].get());
    }
    else
    {
        return SetupCompositeGraphics(resourceName, composite, graphicsEntities);
    }
}

//------------------------------------------------------------------------------
/**
    Transfer transformation from complex physics entity to graphics
    entities.
*/
bool
PhysicsGfxUtil::UpdateGraphicsTransforms(Physics::Entity* physicsEntity, const nArray<Ptr<Graphics::Entity> >& graphicsEntities)
{
    n_assert(physicsEntity);
    Physics::Composite* composite = physicsEntity->GetComposite();
    if (composite->IsA(Physics::Ragdoll::RTTI))
    {
        return TransferRagdollTransforms(physicsEntity, (Graphics::CharEntity*) graphicsEntities[0].get());
    }
    else
    {
        return TransferCompositeTransforms(composite, graphicsEntities);
    }
}

//------------------------------------------------------------------------------
/**
    Create graphics entities for ragdoll physics. This means we're going
    to create a Graphics::CharEntity, since only this allows access
    to the Nebula2 character object.
*/
nArray<Ptr<Graphics::Entity> >
PhysicsGfxUtil::CreateRagdollGraphics(Physics::Ragdoll* ragdoll)
{
    n_assert(ragdoll);
    nArray<Ptr<Graphics::Entity> > graphicsEntities;
	graphicsEntities.Append(Graphics::Entity::Create());
    return graphicsEntities;
}

//------------------------------------------------------------------------------
/**
    This will create one or several graphics entities which can visualize
    the physics composite. This method is used for complex physics composites
    which may be constructed from several rigid bodies and joints. There
    will be one graphics entity created for each rigid body which has
    a ModelNode link. The method will also take care of optional
    ShadowNode links.
*/
nArray<Ptr<Graphics::Entity> >
PhysicsGfxUtil::CreateCompositeGraphics(Physics::Composite* composite)
{
    n_assert(composite);

    nArray<Ptr<Graphics::Entity> > graphicsEntities;
    if (composite->HasLinkType(Physics::RigidBody::ModelNode))
    {
        int numBodies = composite->GetNumBodies();
        int bodyIndex;
        int visualIndex = 0;
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            Physics::RigidBody* body = composite->GetBodyAt(bodyIndex);
            if (body->IsLinkValid(Physics::RigidBody::ModelNode))
            {
                body->SetLinkIndex(graphicsEntities.Size());
				graphicsEntities.Append(Graphics::Entity::Create());
            }
        }
    }
    else
    {
        // fallback: just create a single entity
		graphicsEntities.Append(Graphics::Entity::Create());
    }
    return graphicsEntities;
}

//------------------------------------------------------------------------------
/**
    Setup graphics entities created by CreateCompositeGraphics(). This
    will initialize the graphics entities resource names, computes
    correctional matrices for the graphics nodes and attaches the
    graphics entities to the level. If nothing has been done (because the
    composite's rigid bodies have no ModelNode links, the method will return false.
*/
bool
PhysicsGfxUtil::SetupCompositeGraphics(const nString& resourceName,
                                       Physics::Composite* composite,
                                       const nArray<Ptr<Graphics::Entity> >& graphicsEntities)
{
    n_assert(resourceName.IsValid());
    n_assert(composite);
    n_assert(graphicsEntities.Size() > 0);

    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);

    if (composite->HasLinkType(Physics::RigidBody::ModelNode))
    {
        int numBodies = composite->GetNumBodies();
        int bodyIndex;
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            Physics::RigidBody* body = composite->GetBodyAt(bodyIndex);
            if (body->IsLinkValid(Physics::RigidBody::ModelNode))
            {
                Graphics::Entity* graphicsEntity = graphicsEntities[body->GetLinkIndex()];
                graphicsEntity->SetTransform(body->GetTransform());

                // setup resource names
                nString modelResName = resourceName;
                modelResName.TrimRight("/");
                modelResName.Append("/");
                modelResName.Append(body->GetLinkName(Physics::RigidBody::ModelNode));
                graphicsEntity->SetResourceName(modelResName);

                if (body->IsLinkValid(Physics::RigidBody::ShadowNode))
                {
                    nString shadowResName = resourceName;
                    shadowResName.TrimRight("/");
                    shadowResName.Append("/");
                    shadowResName.Append(body->GetLinkName(Physics::RigidBody::ShadowNode));
                    graphicsEntity->SetShadowResourceName(shadowResName);
                }

                // attach graphics entity to level, this will validate the resource node objects
                graphicsLevel->AttachEntity(graphicsEntity);

                // compute correctional matrices for the Nebula2 transform nodes,
                // but only once per node!
                nTransformNode* transformNode = graphicsEntity->GetResource().GetNode();
                if (!transformNode->GetLocked())
                {
                    matrix44 invBodyTransform = body->GetInitialTransform();
                    invBodyTransform.invert_simple();
                    const matrix44& nodeTransform = transformNode->GetTransform();
                    matrix44 localNodeTransform = nodeTransform * invBodyTransform;
                    transformNode->SetTransform(localNodeTransform);

                    // do the same for the shadow resource
                    if (body->IsLinkValid(Physics::RigidBody::ShadowNode))
                    {
                        nTransformNode* shadowTransformNode = graphicsEntity->GetShadowResource().GetNode();
                        if (!shadowTransformNode->GetLocked())
                        {
                            const matrix44& shadowNodeTransform = shadowTransformNode->GetTransform();
                            matrix44 shadowNodeLocalTransform = shadowNodeTransform * invBodyTransform;
                            shadowTransformNode->SetTransform(shadowNodeLocalTransform);
                        }
                    }
                }
            }
        }
        return true;
    }
    else
    {
        // setup for simple graphics without physics
        n_assert(graphicsEntities.Size() == 1);
        graphicsEntities[0]->SetResourceName(resourceName);
        graphicsLevel->AttachEntity(graphicsEntities[0]);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Setup for ragdoll graphics. This will actually work the other way
    around and actually setup stuff in the ragdoll object (mainly conversion
    from joint names into joint indices.
*/
bool
PhysicsGfxUtil::SetupRagdollGraphics(const nString& resourceName,
                                     Physics::Entity* physicsEntity,
                                     Graphics::CharEntity* graphicsEntity)
{
    n_assert(physicsEntity);
    n_assert(graphicsEntity && graphicsEntity->IsA(Graphics::CharEntity::RTTI));
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    Physics::Ragdoll* ragdoll = (Physics::Ragdoll*) physicsEntity->GetComposite();
    n_assert(ragdoll && ragdoll->IsA(Physics::Ragdoll::RTTI));

    // first do the usual graphics entity setup stuff
    graphicsEntity->SetTransform(physicsEntity->GetTransform());
    graphicsEntity->SetResourceName(resourceName);
    graphicsLevel->AttachEntity(graphicsEntity);

    // setup the ragdoll
    nCharacter2* nebCharacter = graphicsEntity->GetCharacterPointer();
    n_assert(nebCharacter);
    ragdoll->SetCharacter(nebCharacter);
    ragdoll->Bind();

    return true;
}

//------------------------------------------------------------------------------
/**
    This transfers rigid body transformations from a physics composite
    into graphics entities. Returns false if nothing had been done
    because the composite has no ModelNode links.
*/
bool
PhysicsGfxUtil::TransferCompositeTransforms(Physics::Composite* composite, const nArray<Ptr<Graphics::Entity> >& graphicsEntities)
{
    n_assert(composite);
    n_assert(graphicsEntities.Size() > 0);
    if (composite->HasLinkType(Physics::RigidBody::ModelNode))
    {
        int bodyIndex;
        int numBodies = composite->GetNumBodies();
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            Physics::RigidBody* body = composite->GetBodyAt(bodyIndex);
            if (body->IsLinkValid(Physics::RigidBody::ModelNode))
            {
                int linkIndex = body->GetLinkIndex();
                n_assert(linkIndex >= 0 && linkIndex < graphicsEntities.Size());
                graphicsEntities[body->GetLinkIndex()]->SetTransform(body->GetTransform());
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Transfers the physics positions into the graphics entity for ragdoll
    objects.
*/
bool
PhysicsGfxUtil::TransferRagdollTransforms(Physics::Entity* physicsEntity, Graphics::CharEntity* graphicsEntity)
{
    n_assert(physicsEntity);
    n_assert(graphicsEntity && graphicsEntity->IsA(Graphics::CharEntity::RTTI));

    // first position the graphics entity itself
    graphicsEntity->SetTransform(physicsEntity->GetTransform());

    // then write joint positions to Nebula2 character
    Physics::Ragdoll* ragdoll = (Physics::Ragdoll*) physicsEntity->GetComposite();
    n_assert(ragdoll && ragdoll->IsA(Physics::Ragdoll::RTTI));
    ragdoll->WriteJoints();

    return true;
}

} // namespace Util


