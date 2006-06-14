//------------------------------------------------------------------------------
//  properties/graphicsproperty.h
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/graphicsproperty.h"
#include "properties/abstractphysicsproperty.h"
#include "util/physicsgfxutil.h"
#include "util/segmentedgfxutil.h"
#include "graphics/server.h"
#include "graphics/level.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "msg/updatetransform.h"

namespace Properties
{
ImplementRtti(Properties::GraphicsProperty, AbstractGraphicsProperty);
ImplementFactory(Properties::GraphicsProperty);

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::GraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsProperty::~GraphicsProperty()
{
    n_assert(this->graphicsEntities.Size() == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::SetupDefaultAttributes()
{
    AbstractGraphicsProperty::SetupDefaultAttributes();
    GetEntity()->SetString(Attr::Graphics, "");
}

//------------------------------------------------------------------------------
/**
    Attach the property to a game entity. This will create and setup
    the required graphics entities.
*/
void
GraphicsProperty::OnActivate()
{
    AbstractGraphicsProperty::OnActivate();

    this->SetupGraphicsEntities();
}

//------------------------------------------------------------------------------
/**
    Remove the property from its game entity. This will release the
    graphics entities owned by the property.
*/
void
GraphicsProperty::OnDeactivate()
{
    this->CleanupGraphicsEntities();

    // up to parent class
    AbstractGraphicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method is called before rendering happens and updates the
    positions of the graphics entity.
*/
void
GraphicsProperty::OnRender()
{
    // TODO: DO NOT SEARCH FOR A ATTACHED PHYSICS PROERTY!
    // if we have a physics property, transfer physics transforms to
    // the graphics entities
    AbstractPhysicsProperty* physProperty = (AbstractPhysicsProperty*) GetEntity()->FindProperty(AbstractPhysicsProperty::RTTI);
    if (physProperty && physProperty->IsEnabled())
    {
        Physics::Entity* physEntity = physProperty->GetPhysicsEntity();
        if (physEntity)
        {
            if (Util::PhysicsGfxUtil::UpdateGraphicsTransforms(physEntity, this->graphicsEntities))
            {
                return;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Setup the graphics entities. You may override this method in a subclass
    if different setup is needed.
*/
void
GraphicsProperty::SetupGraphicsEntities()
{
    // get some entity attributes
    nString resName = GetEntity()->GetString(Attr::Graphics);
    const matrix44& worldMatrix = GetEntity()->GetMatrix44(Attr::Transform);

    // TODO: DO NOT SEARCH FOR A ATTACHED PHYSICS PROERTY!
    // check if we have a physics property attached
    AbstractPhysicsProperty* physProperty = (AbstractPhysicsProperty*) GetEntity()->FindProperty(AbstractPhysicsProperty::RTTI);
    if (physProperty && physProperty->IsEnabled())
    {
        // setup graphics entities for visualizing physics
        Physics::Entity* physEntity = physProperty->GetPhysicsEntity();
        if (physEntity)
        {
            this->graphicsEntities = Util::PhysicsGfxUtil::CreateGraphics(physEntity);
            n_assert(this->graphicsEntities.Size() > 0);
            Util::PhysicsGfxUtil::SetupGraphics(resName, physEntity, this->graphicsEntities);
            for (int i = 0; i < this->graphicsEntities.Size(); i++)
            {
                this->graphicsEntities[i]->SetUserData(GetEntity()->GetUniqueId());
            }
            return;
        }
    }

    // fallthrough: setup physics-less graphics entity
    this->graphicsEntities = Util::SegmentedGfxUtil::CreateAndSetupGraphicsEntities(resName, worldMatrix);
    for (int i = 0; i < this->graphicsEntities.Size(); i++)
    {
        this->graphicsEntities[i]->SetUserData(GetEntity()->GetUniqueId());
    }
}

//------------------------------------------------------------------------------
/**
    Cleanup graphics entities
*/
void
GraphicsProperty::CleanupGraphicsEntities() {
    Graphics::Level* gfxLevel = Graphics::Server::Instance()->GetLevel();

    // release graphics entities
    int i;
    int num = this->graphicsEntities.Size();
    for (i = 0;  i < num; i++)
    {
        Graphics::Entity* gfxEntity = this->graphicsEntities[i];
        if (gfxEntity->GetCell())
        {
            gfxLevel->RemoveEntity(gfxEntity);
        }
        this->graphicsEntities[i] = 0;
    }
    this->graphicsEntities.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::UpdateTransform::Id)) return true;
    return AbstractGraphicsProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsProperty::HandleMessage(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::UpdateTransform::Id))
    {
        // update the graphics entities transform
        Message::UpdateTransform* updateTransform = (Message::UpdateTransform*) msg;
        int i;
        int num = this->graphicsEntities.Size();
        for (i = 0; i < num; i++)
        {
            this->graphicsEntities[i]->SetTransform(updateTransform->GetMatrix());
        }
    }
    else
    {
        AbstractGraphicsProperty::HandleMessage(msg);
    }
}

}; // namespace Properties
