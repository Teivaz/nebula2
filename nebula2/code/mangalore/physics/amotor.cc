//------------------------------------------------------------------------------
//  physics/amotor.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "physics/amotor.h"

namespace Physics
{
ImplementRtti(Physics::AMotor, Physics::Joint);
ImplementFactory(Physics::AMotor);

//------------------------------------------------------------------------------
/**
*/
AMotor::AMotor() :
    Joint(Joint::AMotor)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AMotor::~AMotor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
AMotor::Attach(dWorldID worldID, dJointGroupID groupID, const matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateAMotor(worldID, groupID);

    // configure ODE joint
    dJointSetAMotorMode(this->odeJointId, dAMotorUser);
    dJointSetAMotorNumAxes(this->odeJointId, this->GetNumAxes());
    int i;
    int num = this->GetNumAxes();
    for (i = 0; i < num; i++)
    {
        const JointAxis& curAxis = this->axisParams[i];
        dJointSetAMotorAngle(this->odeJointId, i, curAxis.GetAngle());
        if (curAxis.IsLoStopEnabled())
        {
            dJointSetAMotorParam(this->odeJointId, dParamLoStop + dParamGroup * i, curAxis.GetLoStop());
        }
        if (curAxis.IsHiStopEnabled())
        {
            dJointSetAMotorParam(this->odeJointId, dParamHiStop + dParamGroup * i, curAxis.GetHiStop());
        }
        dJointSetAMotorParam(this->odeJointId, dParamVel + dParamGroup * i, curAxis.GetVelocity());
        dJointSetAMotorParam(this->odeJointId, dParamFMax + dParamGroup * i, curAxis.GetFMax());
        dJointSetAMotorParam(this->odeJointId, dParamFudgeFactor + dParamGroup * i, curAxis.GetFudgeFactor());
        dJointSetAMotorParam(this->odeJointId, dParamBounce + dParamGroup * i, curAxis.GetBounce());
        dJointSetAMotorParam(this->odeJointId, dParamCFM + dParamGroup * i, curAxis.GetCFM());
        dJointSetAMotorParam(this->odeJointId, dParamStopERP + dParamGroup * i, curAxis.GetStopERP());
        dJointSetAMotorParam(this->odeJointId, dParamStopCFM + dParamGroup * i, curAxis.GetStopCFM());
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
AMotor::UpdateTransform(const matrix44& m)
{
    matrix33 m33(m.x_component(), m.y_component(), m.z_component());
    int i;
    int num = this->GetNumAxes();
    for (i = 0; i < num; i++)
    {
        vector3 a = m33 * this->axisParams[i].GetAxis();
        dJointSetAMotorAxis(this->odeJointId, i, 0, a.x, a.y, a.z);
    }
}

} // namespace Physics
