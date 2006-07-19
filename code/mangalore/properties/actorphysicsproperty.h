#ifndef PROPERTIES_ACTORPHYSICSPROPERTY_H
#define PROPERTIES_ACTORPHYSICSPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class Properties::ActorPhysicsProperty

    ActorPhysicsProperty adds "actor physics" to an entity. This is
    mainly a capsule which is always kept upright and responds "immediately"
    to move messages. Attach the ActorPhysicsProperty instead of a
    PhysicsProperty to an entity.

    ActorPhysicsProperty implements the following messages:

    MoveGoto
    MoveFollow
    MoveDirection
    MoveStop
    MoveTeleport
    MoveTurn
    MoveRotate

    (C) 2005 Radon Labs GmbH
*/
#include "properties/abstractphysicsproperty.h"
#include "msg/movegoto.h"
#include "msg/movedirection.h"
#include "msg/movefollow.h"
#include "msg/movestop.h"
#include "msg/settransform.h"
#include "msg/moveturn.h"
#include "msg/moverotate.h"
#include "game/entity.h"
#include "physics/charentity.h"
#include "util/npfeedbackloop.h"
#include "util/nangularpfeedbackloop.h"
#include "navigation/path3d.h"

//------------------------------------------------------------------------------
namespace Properties
{
class ActorPhysicsProperty : public AbstractPhysicsProperty
{
    DeclareRtti;
	DeclareFactory(ActorPhysicsProperty);

public:
    /// constructor
    ActorPhysicsProperty();
    /// destructor
    virtual ~ActorPhysicsProperty();

    /// setup default entity attributes
    virtual void SetupDefaultAttributes();

    /// called before movement has happened
    virtual void OnMoveBefore();
    /// called after movement has happened
    virtual void OnMoveAfter();
    /// called on debug visualization
    virtual void OnRenderDebug();

    /// get a pointer to the physics entity
    virtual Physics::Entity* GetPhysicsEntity() const;

    /// return true if message is accepted by controller
    virtual bool Accepts(Message::Msg* msg);
    /// handle a single message
    virtual void HandleMessage(Message::Msg* msg);

protected:
    /// enable and activate the physics
    virtual void EnablePhysics();
    /// disable and cleanup the physics
    virtual void DisablePhysics();

    /// immediately stop the entity
    void Stop();
    /// send a synchronous MoveStop message to self
    void SendStop();
    /// handle a MoveDirection message
    void HandleMoveDirection(Message::MoveDirection* msg);
    /// handle a MoveGoto message
    void HandleMoveGoto(Message::MoveGoto* msg);
    /// handle a SetTransform message
    void HandleSetTransform(Message::SetTransform* msg);
    /// handle a MoveTurn message
    void HandleMoveTurn(Message::MoveTurn* msg);
    /// handle a MoveRotate message
    void HandleMoveRotate(Message::MoveRotate* msg);
    /// handle a MoveFollow message
    void HandleMoveFollow(Message::MoveFollow* msg);
    /// return true if Goto is currently active
    bool IsGotoActive() const;
    /// continue a Goto
    void ContinueGoto();
    /// continue a Follow
    void ContinueFollow();

    Ptr<Physics::CharEntity> charPhysicsEntity;

    nPFeedbackLoop<vector3> smoothedPosition;
    nAngularPFeedbackLoop smoothedHeading;

    float gotoTargetDist;
    float followTargetDist;
    Ptr<Navigation::Path3D> gotoPath;
    int curGotoSegment;
    nTime gotoTimeStamp;
    float headingGain;
    float positionGain;
};

RegisterFactory(ActorPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
ActorPhysicsProperty::IsGotoActive() const
{
    return this->gotoPath.isvalid();
}

} // namespace Properties
//------------------------------------------------------------------------------
#endif
