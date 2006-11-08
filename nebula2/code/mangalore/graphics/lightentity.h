#ifndef GRAPHICS_LIGHTENTITY_H
#define GRAPHICS_LIGHTENTITY_H
//------------------------------------------------------------------------------
/**
    @class Graphics::LightEntity

    A graphics entity which emits light into its surroundings.
    A light entity does not need an existing Nebula2 object to work, it will
    just create it's own light.

    (C) 2003 RadonLabs GmbH
*/
#include "graphics/entity.h"
#include "gfx2/nlight.h"

class nLightNode;

//------------------------------------------------------------------------------
namespace Graphics
{
class LightEntity : public Entity
{
    DeclareRtti;
	DeclareFactory(LightEntity);

public:
    /// constructor
    LightEntity();
    /// destructor
    virtual ~LightEntity();
    /// get entity type
    virtual Entity::Type GetType() const;
    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// render the light, this renders all objects visible to this light
    virtual void Render();
    /// get clip status against bounding box
    virtual ClipStatus GetBoxClipStatus(const bbox3& box);
    /// set light description
    void SetLight(const nLight& l);
    /// get light description
    const nLight& GetLight() const;

private:
    /// update Nebula light attribute from stored attributes
    void UpdateNebulaLight();

    nLight light;
    nRef<nLightNode> refLightNode;
    static int uniqueLightId;
};

RegisterFactory(LightEntity);

//------------------------------------------------------------------------------
/**
*/
inline
void
LightEntity::SetLight(const nLight& l)
{
    this->light = l;
    if (this->refLightNode.isvalid())
    {
        this->UpdateNebulaLight();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const nLight&
LightEntity::GetLight() const
{
    return this->light;
}

} // namespace Graphics
//------------------------------------------------------------------------------
#endif
