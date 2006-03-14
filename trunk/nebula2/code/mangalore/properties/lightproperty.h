#ifndef PROPERTIES_LIGHTPROPERTY_H
#define PROPERTIES_LIGHTPROPERTY_H
//------------------------------------------------------------------------------
/**
    A light property adds a light source object (Graphics::LightEntity) 
    to a game entity.
    
    (C) 2005 Radon Labs GmbH
*/
#include "game/property.h"
#include "graphics/lightentity.h"
#include "attr/attributes.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(LightType);
    DeclareVector4(LightColor);
    DeclareVector3(LightPos);
    DeclareFloat(LightRange);
    DeclareVector4(LightRot);    // a quaternion
    DeclareVector4(LightAmbient);
    DeclareBool(LightCastShadows);
};
//------------------------------------------------------------------------------
namespace Properties
{
class LightProperty : public Game::Property
{
    DeclareRtti;
	DeclareFactory(LightProperty);

public:
    /// constructor
    LightProperty();
    /// destructor
    virtual ~LightProperty();
    /// not active for sleeping entities
    virtual int GetActiveEntityPools() const;
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called before rendering
    virtual void OnRender();
    /// set light parameters
    void SetLight(const nLight& l);
    /// get light properties
    const nLight& GetLight() const;

private:
    Ptr<Graphics::LightEntity> lightEntity;
    nLight light;    
};

RegisterFactory(LightProperty);

//------------------------------------------------------------------------------
/**
*/
inline
void
LightProperty::SetLight(const nLight& l)
{
    this->light = l;
    if (this->lightEntity.isvalid())
    {
        this->lightEntity->SetLight(l);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const nLight&
LightProperty::GetLight() const
{
    return this->light;
}

}; // namespace Properties
//------------------------------------------------------------------------------
#endif
