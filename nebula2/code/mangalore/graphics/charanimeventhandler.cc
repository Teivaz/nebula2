//------------------------------------------------------------------------------
//  graphics/charanimeventhandler.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "graphics/charanimeventhandler.h"
#include "graphics/entity.h"
#include "msg/playsound.h"
#include "msg/playvisualeffect.h"
#include "foundation/factory.h"

namespace Graphics
{

//------------------------------------------------------------------------------
/**
*/
CharAnimEventHandler::CharAnimEventHandler() :
    lastSoundTime(0.0),
    lastVfxTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharAnimEventHandler::~CharAnimEventHandler()
{
    this->entity = 0;
}

//------------------------------------------------------------------------------
/**
    FIXME: It can happen that events are generated although the game is
    currently paused. When the game is paused, the time stamp will
    remain at one position and the events will be fired each frame.
*/
void
CharAnimEventHandler::HandleEvent(const nAnimEventTrack& track, int eventIndex)
{
    const nAnimEvent& event = track.GetEvent(eventIndex);

    // get global position of event, apply the Nebula2 180 degree hack for characters
    matrix44 rot180;
    rot180.rotate_y(n_deg2rad(180.0f));

    matrix44 m;
    m.mult_simple(matrix44(event.GetQuaternion()));
    m.translate(event.GetTranslation());
    m.mult_simple(rot180);
    m.mult_simple(this->entity->GetTransform());
    const vector3& globalPos = m.pos_component();

    this->FireSound(track.GetName(), globalPos);
    this->FireVFX(track.GetName(), m);
}

//------------------------------------------------------------------------------
/**
    Create an audio effect for the event.
    Override to implement a more specific vfx effect handling.
*/
void
CharAnimEventHandler::FireSound(const nString& event, const vector3& position)
{
    nTime curTime = this->entity->GetEntityTime();
    if (this->lastSoundTime != curTime)
    {
		Ptr<Message::PlaySound> msg = Message::PlaySound::Create();
        msg->SetName(event.Get());
        msg->SetPosition(position);
        msg->BroadcastAsync();
        this->lastSoundTime = curTime;
    }
}

//------------------------------------------------------------------------------
/**
    Create a visual effect for the event.
    Override to implement a more specific vfx effect handling.
*/
void
CharAnimEventHandler::FireVFX(const nString& event, const matrix44& transform)
{
    nTime curTime = this->entity->GetEntityTime();
    if (this->lastVfxTime != curTime)
    {
		Ptr<Message::PlayVisualEffect> msg = Message::PlayVisualEffect::Create();
        msg->SetName(event.Get());
        msg->SetTransform(transform);
        msg->BroadcastAsync();
        this->lastVfxTime = curTime;
    }
}

};
