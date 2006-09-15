//------------------------------------------------------------------------------
//  properties/actorphysicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "properties/actorphysicsproperty.h"
#include "foundation/factory.h"
#include "attr/attributes.h"
#include "physics/server.h"
#include "physics/level.h"
#include "game/time/gametimesource.h"
#include "mathlib/polar.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "managers/entitymanager.h"
#include "navigation/server.h"
#include "msg/movesetvelocity.h"
#include "gfx2/ngfxserver2.h"

namespace Properties
{
ImplementRtti(Properties::ActorPhysicsProperty, Properties::AbstractPhysicsProperty);
ImplementFactory(Properties::ActorPhysicsProperty);

using namespace Game;
using namespace Message;
using namespace Managers;

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsProperty::ActorPhysicsProperty() :
    followTargetDist(4.0f),
    gotoTargetDist(0.2f),
    curGotoSegment(0),
    gotoTimeStamp(0.0),
    headingGain(-6.0f),
    positionGain(-25.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsProperty::~ActorPhysicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetupDefaultAttributes()
{
    AbstractPhysicsProperty::SetupDefaultAttributes();
    GetEntity()->SetVector3(Attr::VelocityVector, vector3(0.0f, 0.0f, 0.0f));
    GetEntity()->SetFloat(Attr::RelVelocity, 1.0f);
    GetEntity()->SetFloat(Attr::MaxVelocity, 2.5f);
    GetEntity()->SetBool(Attr::Following, false);
    GetEntity()->SetBool(Attr::Moving, false);
}

//------------------------------------------------------------------------------
/**
    Creates a Physics::CharEntity instead of a normal Physics::Entity.
*/
void
ActorPhysicsProperty::EnablePhysics()
{
    n_assert(!this->IsEnabled());

    // create a char physics entity
	this->charPhysicsEntity = Physics::CharEntity::Create();
    this->charPhysicsEntity->SetUserData(GetEntity()->GetUniqueId());
    if (GetEntity()->HasAttr(Attr::Physics))
    {
        this->charPhysicsEntity->SetCompositeName(GetEntity()->GetString(Attr::Physics));
    }
    this->charPhysicsEntity->SetTransform(GetEntity()->GetMatrix44(Attr::Transform));

    // attach physics entity to physics level
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    physicsLevel->AttachEntity(this->charPhysicsEntity);

    // make sure we are standing still
    this->Stop();

    // initialize feedback loops for motion smoothing
    nTime time = GameTimeSource::Instance()->GetTime();
    const matrix44& entityMatrix = GetEntity()->GetMatrix44(Attr::Transform);
    this->smoothedPosition.Reset(time, 0.001f, this->positionGain, entityMatrix.pos_component());

    polar2 headingAngle(entityMatrix.z_component());
    this->smoothedHeading.Reset(time, 0.001f, this->headingGain, headingAngle.rho);

    // call parent
    AbstractPhysicsProperty::EnablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());

    // stop
    this->Stop();

    // remove from level
    Physics::Level* physicsLevel = Physics::Server::Instance()->GetLevel();
    n_assert(physicsLevel);
    physicsLevel->RemoveEntity(this->charPhysicsEntity);

    // cleanup resource
    this->charPhysicsEntity = 0;

    // call parent
    AbstractPhysicsProperty::DisablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
bool
ActorPhysicsProperty::Accepts(Message::Msg* msg)
{
    return msg->CheckId(MoveDirection::Id) ||
           msg->CheckId(MoveFollow::Id) ||
           msg->CheckId(MoveGoto::Id) ||
           msg->CheckId(MoveStop::Id) ||
           msg->CheckId(SetTransform::Id) ||
           msg->CheckId(MoveTurn::Id) ||
           msg->CheckId(MoveSetVelocity::Id) ||
           msg->CheckId(MoveRotate::Id) ||
           AbstractPhysicsProperty::Accepts(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::HandleMessage(Message::Msg* msg)
{
    if (!this->IsEnabled())
    {
        // disabled, don't listen to messages
        AbstractPhysicsProperty::HandleMessage(msg);
    }
    else if (msg->CheckId(MoveDirection::Id))
    {
        this->HandleMoveDirection((MoveDirection*) msg);
    }
    else if (msg->CheckId(MoveFollow::Id))
    {
        this->HandleMoveFollow((MoveFollow*) msg);
    }
    else if (msg->CheckId(MoveGoto::Id))
    {
        this->HandleMoveGoto((MoveGoto*) msg);
    }
    else if (msg->CheckId(MoveStop::Id))
    {
        this->Stop();
    }
    else if (msg->CheckId(SetTransform::Id))
    {
        this->HandleSetTransform((SetTransform*) msg);
    }
    else if (msg->CheckId(MoveTurn::Id))
    {
        this->HandleMoveTurn((MoveTurn*) msg);
    }
    else if (msg->CheckId(MoveRotate::Id))
    {
        this->HandleMoveRotate((MoveRotate*) msg);
    }
    else if (msg->CheckId(MoveSetVelocity::Id))
    {
        GetEntity()->SetFloat(Attr::RelVelocity, ((MoveSetVelocity*)msg)->GetRelVelocity());
    }
    else
    {
        AbstractPhysicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    The OnMoveBefore() method handles all pending messages, and other
    per-frame-stuff that must happen before the physics subsystem is
    triggered.
*/
void
ActorPhysicsProperty::OnMoveBefore()
{
    this->HandlePendingMessages();
    if (this->IsEnabled())
    {
        if (this->IsGotoActive())
        {
            this->ContinueGoto();
        }
        if (GetEntity()->GetBool(Attr::Following))
        {
            this->ContinueFollow();
        }
    }
}

//------------------------------------------------------------------------------
/**
    The OnMoveAfter() method transfers the current physics entity transform to
    the game entity.
*/
void
ActorPhysicsProperty::OnMoveAfter()
{
    if (this->IsEnabled())
    {
        // get current physics entity transform and velocity
        if (this->charPhysicsEntity->HasTransformChanged())
        {
            matrix44 physicsEntityTransform = this->charPhysicsEntity->GetTransform();
            vector3 physicsEntityVelocity = this->charPhysicsEntity->GetVelocity();

            // feed the feedback loops
            polar2 headingAngles(physicsEntityTransform.z_component());
            this->smoothedPosition.SetGoal(physicsEntityTransform.pos_component());
            this->smoothedHeading.SetGoal(headingAngles.rho);

            // evaluate the feedback loops
            nTime time = GameTimeSource::Instance()->GetTime();
            this->smoothedPosition.Update(time);
            this->smoothedHeading.Update(time);

            // construct the new entity matrix
            matrix44 entityMatrix;
            entityMatrix.rotate_y(this->smoothedHeading.GetState());
            entityMatrix.translate(this->smoothedPosition.GetState());

            // update game entity
            Ptr<Message::UpdateTransform> msg = Message::UpdateTransform::Create();
            msg->SetMatrix(entityMatrix);
            this->GetEntity()->SendSync(msg);
            this->GetEntity()->SetVector3(Attr::VelocityVector, physicsEntityVelocity);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This simply sends a synchronous stop message to myself. This uses
    a message so that everybody else who might be interested in the information
    that I have stopped can listen to the message.
*/
void
ActorPhysicsProperty::SendStop()
{
    Ptr<MoveStop> msg = MoveStop::Create();
    GetEntity()->SendSync(msg);
}

//------------------------------------------------------------------------------
/**
    Immediately stop the entity.

    26-Jan-06   floh    bugfix: also canceled MoveFollow
    14-Feb-06   nico    bugfix: now really canceled MoveFollow ;)
*/
void
ActorPhysicsProperty::Stop()
{
    this->charPhysicsEntity->SetDesiredVelocity(vector3(0.0f, 0.0f, 0.0f));
    this->gotoPath = 0;
    GetEntity()->SetBool(Attr::Moving, false);
    GetEntity()->SetBool(Attr::Following, false);
    GetEntity()->SetVector3(Attr::VelocityVector, vector3(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
    Handle a MoveDirection message.
*/
void
ActorPhysicsProperty::HandleMoveDirection(MoveDirection* msg)
{
    n_assert(msg);

    vector3 dir = msg->GetDirection();

    // convert camera relative vector into absolute vector if necessary
    if (msg->GetCameraRelative())
    {
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera);
        matrix44 camTransform = camera->GetTransform();
        camTransform.pos_component().set(0.0f, 0.0f, 0.0f);
        dir = camTransform * dir;
    }
    dir.y = 0.0f;
    dir.norm();

    vector3 desiredVelocity = dir * GetEntity()->GetFloat(Attr::RelVelocity) * GetEntity()->GetFloat(Attr::MaxVelocity);
    this->charPhysicsEntity->SetDesiredVelocity(desiredVelocity);
    if (msg->GetCorrespondPose()) {
        this->charPhysicsEntity->SetDesiredLookat(dir);
    }
    GetEntity()->SetBool(Attr::Moving, true);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveTurn message.
*/
void
ActorPhysicsProperty::HandleMoveTurn(MoveTurn* msg)
{
    n_assert(msg);
    vector3 dir = msg->GetDirection();
    if (msg->GetCameraRelative())
    {
        Graphics::CameraEntity* camera = Graphics::Server::Instance()->GetCamera();
        n_assert(camera);
        matrix44 camTransform = camera->GetTransform();
        camTransform.pos_component().set(0.0f, 0.0f, 0.0f);
        dir = camTransform * dir;
    }
    dir.y = 0.0f;
    dir.norm();
    this->charPhysicsEntity->SetDesiredLookat(dir);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveRotate message.
*/
void
ActorPhysicsProperty::HandleMoveRotate(MoveRotate* msg)
{
    n_assert(msg);

    float angle = msg->GetAngle();
    vector3 direction = this->charPhysicsEntity->GetDesiredLookat();
    direction.rotate(vector3(0.f, 1.f, 0.f), angle);
    this->charPhysicsEntity->SetDesiredLookat(direction);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveGoto message.
*/
void
ActorPhysicsProperty::HandleMoveGoto(MoveGoto* msg)
{
    n_assert(msg);

    // make a navigation path from current to target position
    const vector3& from = GetEntity()->GetMatrix44(Attr::Transform).pos_component();
    const vector3& to = msg->GetPosition();
    this->gotoPath = Navigation::Server::Instance()->MakePath(from, to);
    this->curGotoSegment = 0;
    this->gotoTargetDist = msg->GetDistance();

    this->gotoTimeStamp = GameTimeSource::Instance()->GetTime();
    GetEntity()->SetBool(Attr::Moving, true);
}

//------------------------------------------------------------------------------
/**
    Handle a SetTransform message.
*/
void
ActorPhysicsProperty::HandleSetTransform(SetTransform* msg)
{
    n_assert(msg);
    this->charPhysicsEntity->SetTransform(msg->GetMatrix());

    // reset the feedback loops
    nTime time = GameTimeSource::Instance()->GetTime();
    this->smoothedPosition.Reset(time, 0.001f, this->positionGain, msg->GetMatrix().pos_component());
    polar2 headingAngle(-msg->GetMatrix().z_component());
    this->smoothedHeading.Reset(time, 0.001f, this->headingGain, headingAngle.rho);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveFollow message.
*/
void
ActorPhysicsProperty::HandleMoveFollow(MoveFollow* msg)
{
    n_assert(msg);
    GetEntity()->SetInt(Attr::TargetEntityId, msg->GetTargetEntityId());
    if (EntityManager::Instance()->ExistsEntityById(this->GetEntity()->GetInt(Attr::TargetEntityId)))
    {
        Entity* targetEntity = EntityManager::Instance()->GetEntityById(this->GetEntity()->GetInt(Attr::TargetEntityId));
        n_assert(targetEntity);
        this->followTargetDist = msg->GetDistance();
        GetEntity()->SetBool(Attr::Following, true);
    }
}

//------------------------------------------------------------------------------
/**
    Continue the current Goto action.
*/
void
ActorPhysicsProperty::ContinueGoto()
{
    n_assert(this->gotoPath.isvalid());

    const vector3 curPos = GetEntity()->GetMatrix44(Attr::Transform).pos_component();
    vector3 targetVec = this->gotoPath->GetPoints()[this->curGotoSegment] - curPos;
    targetVec.y = 0.0f; // ignore vertical dimension
    float dist = targetVec.len();

    // current segment position reached?
    if (dist < this->gotoTargetDist)
    {
        // reached final target position?
        if (this->curGotoSegment == this->gotoPath->CountSegments())
        {
            this->SendStop();
        }
        else
        {
            // nope, advance to next path segment
            this->curGotoSegment++;
        }
    }
    else
    {
        // just continue to go towards current segment position
        Ptr<MoveDirection> msg = MoveDirection::Create();
        msg->SetDirection(targetVec);
        GetEntity()->SendSync(msg);
    }
}

//------------------------------------------------------------------------------
/**
    Continue the current Follow action.
*/
void
ActorPhysicsProperty::ContinueFollow()
{
    n_assert(GetEntity()->GetBool(Attr::Following));

    if (!EntityManager::Instance()->ExistsEntityById(this->GetEntity()->GetInt(Attr::TargetEntityId)))
    {
        // our target entity has gone...
        this->SendStop();
    }
    else
    {
        Game::Entity* targetEntity = EntityManager::Instance()->GetEntityById(this->GetEntity()->GetInt(Attr::TargetEntityId));
        n_assert(targetEntity);

        // compute positions in world coordinates
        const vector3 targetPos = targetEntity->GetMatrix44(Attr::Transform).pos_component();
        const vector3 curPos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
        vector3 targetVec = targetPos - curPos;
        targetVec.y = 0.0f; // ignore vertical dimension
        float targetDist = targetVec.len();

        // if we are close enough to our target, stop, and always face our target
        if (targetDist < this->followTargetDist)
        {
            // stop and look at our target
            this->SendStop();
            this->charPhysicsEntity->SetDesiredLookat(targetVec);
        }
        // continue following target position if not stopped.
        else if ((this->gotoTimeStamp + 1.0) < GameTimeSource::Instance()->GetTime())
        {
            // continue moving towards our target entity
		    Ptr<MoveGoto> moveGoto = MoveGoto::Create();
            moveGoto->SetPosition(targetPos);
            this->HandleMoveGoto(moveGoto);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Physics::Entity*
ActorPhysicsProperty::GetPhysicsEntity() const
{
    return this->charPhysicsEntity;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the current 3d navigation path.
*/
void
ActorPhysicsProperty::OnRenderDebug()
{
    if (this->gotoPath.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        const nArray<vector3>& points = this->gotoPath->GetPoints();
        for (int i = 0; i < points.Size(); i++)
        {
            matrix44 m;
            m.scale(vector3(0.1f, 0.1f, 0.1f));
            m.translate(points[i] + vector3(0.0f, 0.1f, 0.0f));
            gfxServer->DrawShape(nGfxServer2::Box, m, vector4(1.0f, 0.0f, 0.0f, 0.5f));
        }
    }
}

} // namespace Properties
