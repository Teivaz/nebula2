//------------------------------------------------------------------------------
//  properties/lightproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/lightproperty.h"
#include "foundation/factory.h"
#include "game/entity.h"
#include "attr/attributes.h"
#include "graphics/server.h"
#include "graphics/level.h"

namespace Attr
{
    DefineString(LightType);
    DefineVector4(LightColor);
    DefineVector3(LightPos);
    DefineFloat(LightRange);
    DefineVector4(LightRot);    // a quaternion
    DefineVector4(LightAmbient);
    DefineBool(LightCastShadows);
};

namespace Properties
{
ImplementRtti(Properties::LightProperty, Game::Property);
ImplementFactory(Properties::LightProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
LightProperty::LightProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LightProperty::~LightProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/** 
    Makes only sense for live entities.
*/
int
LightProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
LightProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetString(Attr::LightType, "Point");
    GetEntity()->SetVector4(Attr::LightColor, vector4(1.0f, 1.0f, 1.0f, 1.0f));
    GetEntity()->SetFloat(Attr::LightRange, 10.0f);
    GetEntity()->SetVector4(Attr::LightAmbient, vector4(0.0f, 0.0f, 0.0f, 0.0f));
    GetEntity()->SetBool(Attr::LightCastShadows, false);
}

//------------------------------------------------------------------------------
/**
*/
void
LightProperty::OnActivate()
{
    Property::OnActivate();

    // update Nebula2 light from attributes
    this->light.SetType(nLight::StringToType(GetEntity()->GetString(Attr::LightType).Get()));
    this->light.SetDiffuse(GetEntity()->GetVector4(Attr::LightColor));
    this->light.SetSpecular(this->light.GetDiffuse());
    this->light.SetCastShadows(GetEntity()->GetBool(Attr::LightCastShadows));
    if (this->light.GetType() == nLight::Point)
    {
        this->light.SetRange(GetEntity()->GetFloat(Attr::LightRange));
    }
    else
    {
        this->light.SetAmbient(GetEntity()->GetVector4(Attr::LightAmbient));
    }

    // create and setup graphics light entity
	this->lightEntity = Graphics::LightEntity::Create();
    this->lightEntity->SetLight(this->light);
    this->lightEntity->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));

    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    n_assert(graphicsLevel);
    graphicsLevel->AttachEntity(this->lightEntity);
}

//------------------------------------------------------------------------------
/**
*/
void
LightProperty::OnDeactivate()
{
    // cleanup graphics light property
    Graphics::Level* gfxLevel = Graphics::Server::Instance()->GetLevel();
    gfxLevel->RemoveEntity(this->lightEntity);
    this->lightEntity = 0;

    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
LightProperty::OnRender()
{
    // update our light's position from our game entity
    this->lightEntity->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));
    Property::OnRender();
}

}; // namespace Properties
