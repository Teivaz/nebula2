#ifndef VFX_GRAPHICSEFFECT_H
#define VFX_GRAPHICSEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::GraphicsEffect

    An effect which renders a graphics entity at its position.

    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"
#include "foundation/ptr.h"
#include "graphics/entity.h"

//------------------------------------------------------------------------------
namespace VFX
{
class GraphicsEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(GraphicsEffect);
public:
    /// constructor
    GraphicsEffect();
    /// destructor
    virtual ~GraphicsEffect();
    /// set graphics resource name (category/object)
    void SetResourceName(const nString& n);
    /// get graphics resource name
    const nString& GetResourceName() const;
    /// start the effect
    virtual void OnStart();
    /// deactivate the effect
    virtual void OnDeactivate();
    /// trigger the effect
    virtual void OnFrame();

private:
    nString resName;
    Ptr<Graphics::Entity> graphicsEntity;
};

//------------------------------------------------------------------------------
/*
*/
inline
void
GraphicsEffect::SetResourceName(const nString& n)
{
    this->resName = n;
}

//------------------------------------------------------------------------------
/*
*/
inline
const nString&
GraphicsEffect::GetResourceName() const
{
    return this->resName;
}

};
//------------------------------------------------------------------------------
#endif