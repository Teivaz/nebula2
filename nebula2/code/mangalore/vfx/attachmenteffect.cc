//------------------------------------------------------------------------------
//  attachmenteffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/attachmenteffect.h"
#include "msg/gfxaddattachment.h"
#include "msg/gfxremattachment.h"

namespace VFX
{
ImplementRtti(VFX::AttachmentEffect, VFX::Effect);
ImplementFactory(VFX::AttachmentEffect);

using namespace Message;
using namespace Game;

//------------------------------------------------------------------------------
/**
*/
AttachmentEffect::AttachmentEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AttachmentEffect::~AttachmentEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    This just sends a GfxAddAttachment message to my entity in order
    to create a hardpoint attachment.
*/
void
AttachmentEffect::OnStart()
{
    n_assert(this->HasEntity());
    n_assert(this->jointName.IsValid());
    n_assert(this->resourceName.IsValid());

    // call parent class
    Effect::OnStart();

    // add the attachment to my entity
    Ptr<GfxAddAttachment> msg = GfxAddAttachment::Create();
    msg->SetJointName(this->jointName);
    msg->SetResourceName(this->resourceName);
    msg->SetOffsetMatrix(this->offsetMatrix);
    this->entity->SendSync(msg);
}

//------------------------------------------------------------------------------
/**
    This just sends a GfxRemAttachment to my entity in order to
    remove the hardpoint attachment.
*/
void
AttachmentEffect::OnDeactivate()
{
    n_assert(this->HasEntity());
    n_assert(this->jointName.IsValid());

    // remove attachment from entity
    Ptr<GfxRemAttachment> msg = GfxRemAttachment::Create();
    msg->SetJointName(this->jointName);
    this->entity->SendSync(msg);

    Effect::OnDeactivate();
}

} // namespace VFX
