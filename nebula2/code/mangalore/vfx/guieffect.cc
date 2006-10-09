//------------------------------------------------------------------------------
//  guieffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/guieffect.h"

namespace VFX
{
ImplementRtti(VFX::GuiEffect, VFX::Effect);
ImplementFactory(VFX::GuiEffect);

//------------------------------------------------------------------------------
/**
*/
GuiEffect::GuiEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GuiEffect::~GuiEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GuiEffect::OnStart()
{
    n_assert(this->GetResourceName().IsValid());

    // call parent class
    Effect::OnStart();

    // create and initialize window
    this->window = UI::Window::Create();
    this->window->SetResource(this->GetResourceName());
    this->window->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
GuiEffect::OnDeactivate()
{
    if (this->window.isvalid() && this->window->IsOpen())
    {
        this->window->Close();
        this->window = 0;
    }

    // call parent class
    Effect::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
GuiEffect::OnFrame()
{
    Effect::OnFrame();
}

} // namespace VFX
