//------------------------------------------------------------------------------
//  graphicseffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/graphicseffect.h"
#include "graphics/server.h"
#include "graphics/level.h"

namespace VFX
{
ImplementRtti(VFX::GraphicsEffect, VFX::Effect);
ImplementFactory(VFX::GraphicsEffect);

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect::GraphicsEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect::~GraphicsEffect()
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
GraphicsEffect::OnStart()
{
    n_assert(this->GetResourceName().IsValid());

    // call parent class
    Effect::OnStart();

    // create and initialize graphics entity
    this->graphicsEntity = Graphics::Entity::Create();
    this->graphicsEntity->SetResourceName(this->GetResourceName());
    this->graphicsEntity->SetTransform(this->GetTransform());
    Graphics::Server::Instance()->GetLevel()->AttachEntity(this->graphicsEntity);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEffect::OnDeactivate()
{
    if (this->graphicsEntity.isvalid())
    {
        // cleanup graphics entity
        Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
        if (graphicsLevel)
        {
            graphicsLevel->RemoveEntity(this->graphicsEntity);
        }
        this->graphicsEntity = 0;
    }

    // call parent class
    Effect::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEffect::OnFrame()
{
    Effect::OnFrame();
    if (this->IsPlaying())
    {
        this->graphicsEntity->SetTransform(this->ComputeWorldSpaceTransform());
    }
}

}; // namespace VFX
