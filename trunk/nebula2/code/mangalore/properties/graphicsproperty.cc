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
#include "msg/gfxsetvisible.h"

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
    Get a reference to the array of graphics entities
*/
const nArray<Ptr<Graphics::Entity> >&
GraphicsProperty::GetGraphicsEntities() const
{
    return this->graphicsEntities;
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

    // up to parent class
    AbstractGraphicsProperty::OnDeactivate();
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
    nString resName =  this->GetGraphicsResource();

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

    // fall through: setup physics-less graphics entity
    const matrix44& worldMatrix = GetEntity()->GetMatrix44(Attr::Transform);
    Util::SegmentedGfxUtil segGfxUtil;
    this->graphicsEntities = segGfxUtil.CreateAndSetupGraphicsEntities(resName, worldMatrix);
    for (int i = 0; i < this->graphicsEntities.Size(); i++)
    {
        this->graphicsEntities[i]->SetUserData(GetEntity()->GetUniqueId());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsProperty::Accepts(Message::Msg* msg)
{
    n_assert(msg);
    if (msg->CheckId(Message::UpdateTransform::Id)) return true;
    if (msg->CheckId(Message::GfxSetVisible::Id)) return true;
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
        this->UpdateTransform(((Message::UpdateTransform*)msg)->GetMatrix(), false);
    }
    if (msg->CheckId(Message::GfxSetVisible::Id))
    {
        this->SetVisible(((Message::GfxSetVisible*)msg)->GetVisible());
    }
    else
    {
        AbstractGraphicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Called to update the graphics entity's transform.
*/
void
GraphicsProperty::UpdateTransform(const matrix44& m, bool setDirectly)
{
    // TODO: DO NOT SEARCH FOR A ATTACHED PHYSICS PROERTY!

    // if physics property exists, gather transforms from physics property,
    // otherwise use the transform in the UpdateTransform message


    AbstractPhysicsProperty* physProperty = (AbstractPhysicsProperty*) GetEntity()->FindProperty(AbstractPhysicsProperty::RTTI);
    if (setDirectly || (0 == physProperty))
    {
        // set transformation directly
        int i;
        for (i = 0; i < this->graphicsEntities.Size(); i++)
        {
            this->graphicsEntities[i]->SetTransform(m);
        }
    }
    else
    {
        // gather transform from physics entity
        if (physProperty->IsEnabled())
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
}

//------------------------------------------------------------------------------
/**
    Shows or hides all attached graphics entities.
*/
void
GraphicsProperty::SetVisible(bool visible)
{
    int i;
    for (i = 0; i < this->graphicsEntities.Size(); i++)
    {
        this->graphicsEntities[i]->SetVisible(visible);
    }
}

} // namespace Properties
