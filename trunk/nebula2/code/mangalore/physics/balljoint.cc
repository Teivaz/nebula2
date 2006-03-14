//------------------------------------------------------------------------------
//  physics/balljoint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/balljoint.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::BallJoint, Physics::Joint);
ImplementFactory(Physics::BallJoint);

//------------------------------------------------------------------------------
/**
*/
BallJoint::BallJoint() : 
    Joint(Joint::BallJoint)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BallJoint::~BallJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added 
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
BallJoint::Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateBall(worldID, groupID);

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void 
BallJoint::UpdateTransform(const matrix44& m)
{
    vector3 a = m * this->anchor;
    dJointSetBallAnchor(this->odeJointId, a.x, a.y, a.z);
}

//------------------------------------------------------------------------------
/**
*/
void
BallJoint::RenderDebug()
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        matrix44 m;
        dVector3 curAnchor;
        dJointGetBallAnchor(this->odeJointId, curAnchor);
        m.scale(vector3(0.1f, 0.1f, 0.1f));
        m.translate(vector3(curAnchor[0], curAnchor[1], curAnchor[2]));
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics