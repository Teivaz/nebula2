//------------------------------------------------------------------------------
//  graphics/lightentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/lightentity.h"
#include "scene/nlightnode.h"
#include "graphics/server.h"
#include "mathlib/sphere.h"

namespace Graphics
{
ImplementRtti(Graphics::LightEntity, Graphics::Entity);
ImplementFactory(Graphics::LightEntity);

int LightEntity::uniqueLightId = 0;

//------------------------------------------------------------------------------
/**
*/
LightEntity::LightEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
LightEntity::~LightEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Entity::Type
LightEntity::GetType() const
{
    return Light;
}

//------------------------------------------------------------------------------
/**
    Update Nebula light attributes from stored attributes.
*/
void
LightEntity::UpdateNebulaLight()
{
    // initialize the Nebula2 light
    nLightNode* nebLight = this->refLightNode;
    nebLight->SetType(this->light.GetType());
    nebLight->SetCastShadows(this->light.GetCastShadows());
    nebLight->SetVector(nShaderState::LightDiffuse, this->light.GetDiffuse());
    nebLight->SetVector(nShaderState::LightSpecular, this->light.GetSpecular());
    nebLight->SetVector(nShaderState::LightAmbient, this->light.GetAmbient());
    float lightRange;
    if (nLight::Directional == this->light.GetType())
    {
        lightRange = 100000.0f;
    }
    else
    {
        lightRange = this->light.GetRange();
    }
    nebLight->SetLocalBox(bbox3(vector3(0.0f, 0.0f, 0.0f), vector3(lightRange, lightRange, lightRange)));
    nebLight->SetFloat(nShaderState::LightRange, lightRange);
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics object becomes active (i.e.
    when it is attached to a game entity.
    This method creates a Nebula2 light object and places it in the
    resource pool.
*/
void
LightEntity::OnActivate()
{
    n_assert(!this->active);
    n_assert(!this->resource.IsLoaded());
    n_assert(!this->refLightNode.isvalid());

    nKernelServer* kernelServer = nKernelServer::Instance();
    Foundation::Server* fndServer = Foundation::Server::Instance();

    // create an unique category/object name
    nString lightName = "lights/light";
    lightName.AppendInt(this->uniqueLightId++);

    // create a new Nebula2 light
    nRoot* rsrcPool = fndServer->GetResourcePool(Foundation::Server::GraphicsPool);
    kernelServer->PushCwd(rsrcPool);
    this->refLightNode = (nLightNode*) kernelServer->New("nlightnode", lightName.Get());
    kernelServer->PopCwd();
    this->UpdateNebulaLight();
    this->renderContext.SetFlag(nRenderContext::CastShadows, this->light.GetCastShadows());

    // set the light's resource identifier and let Entity::OnActivate() initialize the rest
    this->SetResourceName(lightName.Get());
    Entity::OnActivate();
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics object becomes inactive
    (i.e. when it is removed from a game entity)
*/
void
LightEntity::OnDeactivate()
{
    n_assert(this->refLightNode.isvalid());
    Entity::OnDeactivate();
    this->refLightNode->Release();
    this->refLightNode.invalidate();
}

//------------------------------------------------------------------------------
/**
    Get the clip status against a bounding box in global space. This basically
    checks whether the given bounding box intersects the light volume.

    @param  box     a bounding box in global space
*/
Entity::ClipStatus
LightEntity::GetBoxClipStatus(const bbox3& box)
{
    // if we are a directional light, everything is in our volume
    if (nLight::Directional == this->light.GetType())
    {
        return Inside;
    }
    else if (nLight::Point == this->light.GetType())
    {
        sphere sph(this->GetTransform().pos_component(), this->light.GetRange());
        switch (sph.clipstatus(box))
        {
            case sphere::Inside:
                return Inside;
            case sphere::Clipped:
                return Clipped;
            case sphere::Outside:
            default:
                return Outside;
        }
    }
    else
    {
        n_error("LightEntity::GetBoxClipStatus() Invalid light type!");
        return Outside;
    }
}

//------------------------------------------------------------------------------
/**
    This renders the light source.
*/
void
LightEntity::Render()
{
    // attach light to the scene
    Entity::Render();
}

} // namespace Graphics
