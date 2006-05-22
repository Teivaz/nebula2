//------------------------------------------------------------------------------
//  vfx/effect.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "vfx/effect.h"
#include "graphics/server.h"
#include "graphics/level.h"
#include "graphics/entity.h"

namespace VFX
{
ImplementRtti(VFX::Effect, Foundation::RefCounted);
ImplementFactory(VFX::Effect);

//------------------------------------------------------------------------------
/**
*/
Effect::Effect() :
    duration(0.0),
    curTime(0.0),
    activationTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Effect::~Effect()
{
    if (this->IsActive())
    {
        this->Deactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Activate the effect. This creates a new graphics entity and attaches
    it to the current graphics level.
*/
void
Effect::Activate()
{
    n_assert(!this->IsActive());

    // create a graphics entity
    this->graphicsEntity.create();

    // initialize the graphics entity
    this->graphicsEntity->SetResourceName(this->GetResourceName());
    this->graphicsEntity->SetTransform(this->GetTransform());

    // attach graphics entity to level
    Graphics::Server::Instance()->GetLevel()->AttachEntity(this->graphicsEntity);

    // record the activation time
    this->activationTime = this->curTime;
}

//------------------------------------------------------------------------------
/**
    Deactivate the effect, this releases the graphics entity of the effect.
*/
void
Effect::Deactivate()
{
    n_assert(this->IsActive());

    // remove graphics entity from level
    Graphics::Level* graphicsLevel = Graphics::Server::Instance()->GetLevel();
    if (graphicsLevel)
    {
        graphicsLevel->RemoveEntity(this->graphicsEntity);
    }

    // release the graphics entity
    this->graphicsEntity = 0;
}

//------------------------------------------------------------------------------
/**
    Call the Update() method once per frame. This checks whether the 
    active duration is over, and if yes, returns true. If this is the case,
    the caller should release the effect object.
*/
bool
Effect::Update()
{
    n_assert(this->IsActive());
    this->graphicsEntity->SetTransform(this->GetTransform());

    nTime age = this->curTime - this->activationTime;
    if (age > this->duration)
    {
        return false;
    }
    else
    {
        return true;
    }
}

} // namespace VFX
