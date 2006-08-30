//------------------------------------------------------------------------------
//  physics/hinge2joint.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/hinge2joint.h"
#include "gfx2/ngfxserver2.h"

namespace Physics
{
ImplementRtti(Physics::Hinge2Joint, Physics::Joint);
ImplementFactory(Physics::Hinge2Joint);

//------------------------------------------------------------------------------
/**
*/
Hinge2Joint::Hinge2Joint() :
    Joint(Joint::Hinge2Joint),
    suspErp(0.2f),
    suspCfm(0.0f),
    axisParams(2)
{
    this->axisParams[0].SetAxis(vector3(0.0f, 1.0f, 0.0f));
    this->axisParams[1].SetAxis(vector3(0.0f, 0.0f, 1.0f));
}

//------------------------------------------------------------------------------
/**
*/
Hinge2Joint::~Hinge2Joint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
Hinge2Joint::Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateHinge2(worldID, groupID);

    // configure ODE joint
    int i;
    for (i = 0; i < 2; i++)
    {
        const JointAxis& curAxis = this->axisParams[i];
        if (curAxis.IsLoStopEnabled())
        {
            dJointSetHinge2Param(this->odeJointId, dParamLoStop + dParamGroup * i, curAxis.GetLoStop());
        }
        if (curAxis.IsHiStopEnabled())
        {
            dJointSetHinge2Param(this->odeJointId, dParamHiStop + dParamGroup * i, curAxis.GetHiStop());
        }
        dJointSetHinge2Param(this->odeJointId, dParamVel + dParamGroup * i, curAxis.GetVelocity());
        dJointSetHinge2Param(this->odeJointId, dParamFMax + dParamGroup * i, curAxis.GetFMax());
        dJointSetHinge2Param(this->odeJointId, dParamFudgeFactor + dParamGroup * i, curAxis.GetFudgeFactor());
        dJointSetHinge2Param(this->odeJointId, dParamBounce + dParamGroup * i, curAxis.GetBounce());
        dJointSetHinge2Param(this->odeJointId, dParamCFM + dParamGroup * i, curAxis.GetCFM());
        dJointSetHinge2Param(this->odeJointId, dParamStopERP + dParamGroup * i, curAxis.GetStopERP());
        dJointSetHinge2Param(this->odeJointId, dParamStopCFM + dParamGroup * i, curAxis.GetStopCFM());
    }
    dJointSetHinge2Param(this->odeJointId, dParamSuspensionERP, this->suspErp);
    dJointSetHinge2Param(this->odeJointId, dParamSuspensionCFM, this->suspCfm);

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void
Hinge2Joint::UpdateTransform(const matrix44& m)
{
    vector3 p = m * this->GetAnchor();
    dJointSetHinge2Anchor(this->odeJointId, p.x, p.y, p.z);

    matrix33 m33(m.x_component(), m.y_component(), m.z_component());
    vector3 a0 = m33 * this->axisParams[0].GetAxis();
    vector3 a1 = m33 * this->axisParams[1].GetAxis();
    dJointSetHinge2Axis1(this->odeJointId, a0.x, a0.y, a0.z);
    dJointSetHinge2Axis2(this->odeJointId, a1.x, a1.y, a1.z);
}

//------------------------------------------------------------------------------
/**
*/
void
Hinge2Joint::RenderDebug()
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        matrix44 m;
        dVector3 curAnchor;
        dJointGetHinge2Anchor(this->odeJointId, curAnchor);
        m.scale(vector3(0.1f, 0.1f, 0.1f));
        m.translate(vector3(curAnchor[0], curAnchor[1], curAnchor[2]));
        nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics

