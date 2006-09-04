//------------------------------------------------------------------------------
//  physics/universaljoint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/universaljoint.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::UniversalJoint, Physics::Joint);
ImplementFactory(Physics::UniversalJoint);

//------------------------------------------------------------------------------
/**
*/
UniversalJoint::UniversalJoint() :
    Joint(Joint::UniversalJoint),
    axisParams(2)
{
    this->axisParams[0].SetAxis(vector3(0.0f, 0.0f, 1.0f));
    this->axisParams[1].SetAxis(vector3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
UniversalJoint::~UniversalJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
UniversalJoint::Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateUniversal(worldID, groupID);

    // configure ODE joint
    for (int i = 0; i < 2; i++)
    {
        const JointAxis& curAxis = this->axisParams[i];
        if (curAxis.IsLoStopEnabled())
        {
            dJointSetUniversalParam(this->odeJointId, dParamLoStop + dParamGroup * i, curAxis.GetLoStop());
        }
        if (curAxis.IsHiStopEnabled())
        {
            dJointSetUniversalParam(this->odeJointId, dParamHiStop + dParamGroup * i, curAxis.GetHiStop());
        }
        dJointSetUniversalParam(this->odeJointId, dParamVel + dParamGroup * i, curAxis.GetVelocity());
        dJointSetUniversalParam(this->odeJointId, dParamFMax + dParamGroup * i, curAxis.GetFMax());
        dJointSetUniversalParam(this->odeJointId, dParamFudgeFactor + dParamGroup * i, curAxis.GetFudgeFactor());
        dJointSetUniversalParam(this->odeJointId, dParamBounce + dParamGroup * i, curAxis.GetBounce());
        dJointSetUniversalParam(this->odeJointId, dParamCFM + dParamGroup * i, curAxis.GetCFM());
        dJointSetUniversalParam(this->odeJointId, dParamStopERP + dParamGroup * i, curAxis.GetStopERP());
        dJointSetUniversalParam(this->odeJointId, dParamStopCFM + dParamGroup * i, curAxis.GetStopCFM());
    }

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void
UniversalJoint::UpdateTransform(const matrix44& m)
{
    vector3 p = m * this->GetAnchor();
    dJointSetUniversalAnchor(this->odeJointId, p.x, p.y, p.z);

    matrix33 m33(m.x_component(), m.y_component(), m.z_component());
    vector3 a0 = m33 * this->axisParams[0].GetAxis();
    vector3 a1 = m33 * this->axisParams[1].GetAxis();
    dJointSetUniversalAxis1(this->odeJointId, a0.x, a0.y, a0.z);
    dJointSetUniversalAxis2(this->odeJointId, a1.x, a1.y, a1.z);
}

//------------------------------------------------------------------------------
/**
*/
void
UniversalJoint::RenderDebug()
{
    if (this->IsAttached())
    {
        matrix44 m;
        dVector3 curAnchor;
        dJointGetUniversalAnchor(this->odeJointId, curAnchor);
        m.scale(vector3(0.1f, 0.1f, 0.1f));
        m.translate(vector3(curAnchor[0], curAnchor[1], curAnchor[2]));
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics
