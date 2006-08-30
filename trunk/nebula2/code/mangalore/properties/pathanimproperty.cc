//------------------------------------------------------------------------------
//  properties/pathanimproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/pathanimproperty.h"
#include "msg/animplay.h"
#include "msg/animstop.h"
#include "msg/animrewind.h"
#include "msg/settransform.h"
#include "game/entity.h"
#include "anim2/nanimationserver.h"
#include "managers/timemanager.h"

namespace Attr
{
    DefineString(AnimPath);
    DefineBool(AnimRelative);
    DefineBool(AnimLoop);
    DefineBool(AnimPlaying);
};

namespace Properties
{
ImplementRtti(Properties::PathAnimProperty, Game::Property);
ImplementFactory(Properties::PathAnimProperty);

using namespace Game;
using namespace Message;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
PathAnimProperty::PathAnimProperty() :
    animTime(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PathAnimProperty::~PathAnimProperty()
{
    n_assert(!this->refAnimation.isvalid());
}

//------------------------------------------------------------------------------
/**
    Makes only sense for live entities.
*/
int
PathAnimProperty::GetActiveEntityPools() const
{
    return Game::Entity::LivePool;
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::SetupDefaultAttributes()
{
    Property::SetupDefaultAttributes();
    GetEntity()->SetString(Attr::AnimPath, "");
    GetEntity()->SetBool(Attr::AnimRelative, false);
    GetEntity()->SetBool(Attr::AnimLoop, true);
    GetEntity()->SetBool(Attr::AnimPlaying, false);
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::OnActivate()
{
    Property::OnActivate();

    // check if we actually should do some animation...
    const nString& animPath = GetEntity()->GetString(Attr::AnimPath);
    if (animPath.IsValid())
    {
        // create a Nebula2 animation object
        nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(animPath);
        n_assert(anim);
        anim->SetFilename(animPath);
        if (!anim->Load())
        {
            n_error("PathAnimProperty: could not load anim '%s'", animPath.Get());
            return;
        }
        this->refAnimation = anim;

        // save initial position if relative animation is requested
        if (GetEntity()->GetBool(Attr::AnimRelative))
        {
            this->initialMatrix = GetEntity()->GetMatrix44(Attr::Transform);
        }

        // setup anim loop type
        nAnimation::Group::LoopType loopType = nAnimation::Group::Clamp;
        if (GetEntity()->GetBool(Attr::AnimLoop))
        {
            loopType = nAnimation::Group::Repeat;
        }
        this->refAnimation->GetGroupAt(0).SetLoopType(loopType);

        // start playback
        this->Rewind();
        this->Play();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::OnDeactivate()
{
    if (this->refAnimation.isvalid())
    {
        this->refAnimation->Release();
        this->refAnimation.invalidate();
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
bool
PathAnimProperty::Accepts(Message::Msg* msg)
{
    if (this->refAnimation.isvalid())
    {
        return msg->CheckId(AnimPlay::Id) ||
            msg->CheckId(AnimStop::Id) ||
            msg->CheckId(AnimRewind::Id);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::HandleMessage(Message::Msg* msg)
{
    if (this->refAnimation.isvalid())
    {
        if (msg->CheckId(AnimPlay::Id))
        {
            this->Play();
        }
        else if (msg->CheckId(AnimStop::Id))
        {
            this->Stop();
        }
        else if (msg->CheckId(AnimRewind::Id))
        {
            this->Rewind();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method actually moves the entity around.
*/
void
PathAnimProperty::OnMoveBefore()
{
    if (this->refAnimation.isvalid())
    {
        // handle pending messages
        this->HandlePendingMessages();

        // handle animation playback
        this->UpdateAnimation();
    }
}

//------------------------------------------------------------------------------
/**
    Start animation playback at the current time position.
*/
void
PathAnimProperty::Play()
{
    GetEntity()->SetBool(Attr::AnimPlaying, true);
}

//------------------------------------------------------------------------------
/**
    Stop animation playback.
*/
void
PathAnimProperty::Stop()
{
    GetEntity()->SetBool(Attr::AnimPlaying, false);
}

//------------------------------------------------------------------------------
/**
    Rewind animation cursor to beginning.
*/
void
PathAnimProperty::Rewind()
{
    this->animTime = 0.0f;
}

//------------------------------------------------------------------------------
/**
    Handle animation playback.
*/
void
PathAnimProperty::UpdateAnimation()
{
    if (GetEntity()->GetBool(Attr::AnimPlaying))
    {
        // get translation, rotation and scale
        vector4 keys[3];
        this->refAnimation->SampleCurves(this->animTime, 0, 0, 3, &keys[0]);

        // build matrix from translation and rotation
        vector3 translate(keys[0].x, keys[0].y, keys[0].z);
        quaternion quat(keys[1].x, keys[1].y, keys[1].z, keys[1].w);
        matrix44 m;
        m.mult_simple(matrix44(quat));
        m.translate(translate);
        if (GetEntity()->GetBool(Attr::AnimRelative))
        {
            m = m * this->initialMatrix;
        }

        // update entity's transformation
        Ptr<Message::UpdateTransform> msg = Message::UpdateTransform::Create();
        msg->SetMatrix(m);
        this->GetEntity()->SendSync(msg);

        // update animation time
        this->animTime += (float) TimeManager::Instance()->GetFrameTime();
    }
}

} // namespace Properties
