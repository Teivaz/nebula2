//------------------------------------------------------------------------------
//  physics/hingejoint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/hingejoint.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::HingeJoint, Physics::Joint);
ImplementFactory(Physics::HingeJoint);

//------------------------------------------------------------------------------
/**
*/
HingeJoint::HingeJoint() :
    Joint(Joint::HingeJoint)
{
    this->axisParams.SetAxis(vector3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
HingeJoint::~HingeJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
HingeJoint::Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateHinge(worldID, groupID);

    // configure ODE joint
    if (this->axisParams.IsLoStopEnabled())
    {
        dJointSetHingeParam(this->odeJointId, dParamLoStop, this->axisParams.GetLoStop());
    }
    if (this->axisParams.IsHiStopEnabled())
    {
        dJointSetHingeParam(this->odeJointId, dParamHiStop, this->axisParams.GetHiStop());
    }
    dJointSetHingeParam(this->odeJointId, dParamVel, this->axisParams.GetVelocity());
    dJointSetHingeParam(this->odeJointId, dParamFMax, this->axisParams.GetFMax());
    dJointSetHingeParam(this->odeJointId, dParamFudgeFactor, this->axisParams.GetFudgeFactor());
    dJointSetHingeParam(this->odeJointId, dParamBounce, this->axisParams.GetBounce());
    dJointSetHingeParam(this->odeJointId, dParamCFM, this->axisParams.GetCFM());
    dJointSetHingeParam(this->odeJointId, dParamStopERP, this->axisParams.GetStopERP());
    dJointSetHingeParam(this->odeJointId, dParamStopCFM, this->axisParams.GetStopCFM());

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void
HingeJoint::UpdateTransform(const matrix44& m)
{
    vector3 p = m * this->GetAnchor();
    dJointSetHingeAnchor(this->odeJointId, p.x, p.y, p.z);

    matrix33 m33(m.x_component(), m.y_component(), m.z_component());
    vector3 a = m33 * this->axisParams.GetAxis();
    dJointSetHingeAxis(this->odeJointId, a.x, a.y, a.z);
}

//------------------------------------------------------------------------------
/**
*/
void
HingeJoint::RenderDebug()
{
    if (this->IsAttached())
    {
        matrix44 m;
        dVector3 curAnchor;
        dJointGetHingeAnchor(this->odeJointId, curAnchor);
        m.scale(vector3(0.1f, 0.1f, 0.1f));
        m.translate(vector3(curAnchor[0], curAnchor[1], curAnchor[2]));
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics
