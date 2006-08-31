#ifndef VFX_GUIEFFECT_H
#define VFX_GUIEFFECT_H
//------------------------------------------------------------------------------
/**
    @class VFX::GuiEffect

    A graphics effect which is rendered through the 3D GUI.

    (C) 2006 Radon Labs GmbH
*/
#include "vfx/effect.h"
#include "foundation/ptr.h"
#include "ui/window.h"

//------------------------------------------------------------------------------
namespace VFX
{
class GuiEffect : public Effect
{
    DeclareRtti;
    DeclareFactory(GuiEffect);
public:
    /// constructor
    GuiEffect();
    /// destructor
    virtual ~GuiEffect();
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
    Ptr<UI::Window> window;
};

//------------------------------------------------------------------------------
/*
*/
inline
void
GuiEffect::SetResourceName(const nString& n)
{
    this->resName = n;
}

//------------------------------------------------------------------------------
/*
*/
inline
const nString&
GuiEffect::GetResourceName() const
{
    return this->resName;
}

};
//------------------------------------------------------------------------------
#endif