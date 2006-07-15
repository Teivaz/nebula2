#ifndef N_OPENDELAYER_H
#define N_OPENDELAYER_H
//----------------------------------------------------------------------------
/**
    This is an intermediate layer between ODE and Nebula which hides
    data type conversions.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#include "opende/nopendemarshal.h"

//----------------------------------------------------------------------------
namespace nOpende {

//----------------------------------------------------------------------------
/*
    World functions
*/

inline
dWorldID WorldCreate() { return dWorldCreate(); }

inline
void WorldDestroy( dWorldID id ) { dWorldDestroy( id ); }

inline
void WorldSetGravity( dWorldID id, float x, float y, float z )
{
    dWorldSetGravity( id, x, y, z );
}

inline
void WorldSetGravity( dWorldID id, const vector3& g )
{
    dWorldSetGravity( id, g.x, g.y, g.z );
}

inline
void WorldGetGravity( dWorldID id, vector3& gravity )
{
    dVector3 t;
    dWorldGetGravity( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, gravity );
}

inline
void WorldSetERP( dWorldID id, float erp ) { dWorldSetERP( id, erp ); }

inline
float WorldGetERP( dWorldID id ) { return dWorldGetERP( id ); }

inline
void WorldSetCFM( dWorldID id, float cfm ) { dWorldSetCFM( id, cfm ); }

inline
float WorldGetCFM( dWorldID id ) { return dWorldGetCFM( id ); }

inline
void WorldStep( dWorldID id, float stepsize ) { dWorldStep( id, stepsize ); }

inline
void WorldImpulseToForce( dWorldID id, float stepsize,
                          float ix, float iy, float iz, vector3& force )
{
    dVector3 t;
    dWorldImpulseToForce( id, stepsize, ix, iy, iz, t );
    nOpendeMarshal::dVector3_2_vector3( t, force );
}

inline
void WorldImpulseToForce( dWorldID id, float stepsize, const vector3& impulse, 
                          vector3& force )
{
    dVector3 t;
    dWorldImpulseToForce( id, stepsize, impulse.x, impulse.y, impulse.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, force );
}

inline
void WorldSetAutoDisableFlag( dWorldID id, int do_auto_disable )
{
    dWorldSetAutoDisableFlag( id, do_auto_disable );
}

inline 
int WorldGetAutoDisableFlag( dWorldID id )
{
    return dWorldGetAutoDisableFlag( id );
}

inline
void WorldSetAutoDisableLinearThreshold( dWorldID id, float linear_threshold)
{
    dWorldSetAutoDisableLinearThreshold(id, linear_threshold);
}

inline 
float WorldGetAutoDisableLinearThreshold( dWorldID id )
{
    return dWorldGetAutoDisableLinearThreshold( id );
}

inline 
void WorldSetAutoDisableAngularThreshold( dWorldID id, float angular_threshold )
{
    dWorldSetAutoDisableAngularThreshold( id, angular_threshold );
}

inline 
float WorldGetAutoDisableAngularThreshold( dWorldID id )
{
    return dWorldGetAutoDisableAngularThreshold( id );
}

inline
void WorldSetAutoDisableSteps( dWorldID id, int steps )
{
    dWorldSetAutoDisableSteps( id, steps );
}

inline 
int WorldGetAutoDisableSteps( dWorldID id )
{
    return dWorldGetAutoDisableSteps( id );
}

inline
void WorldSetAutoDisableTime( dWorldID id, float time )
{
    dWorldSetAutoDisableTime( id, time );
}

inline 
float WorldGetAutoDisableTime( dWorldID id )
{
    return dWorldGetAutoDisableTime( id );
}

inline
void CloseODE() { dCloseODE(); }

//----------------------------------------------------------------------------
/*
    StepFast functions
*/

inline
void WorldStepFast1( dWorldID id, float stepsize, int maxiterations )
{
    dWorldStepFast1( id, stepsize, maxiterations );
}

//----------------------------------------------------------------------------
/*
    QuickStep functions
*/

inline
void WorldQuickStep( dWorldID id, float stepsize ) 
{ 
    dWorldQuickStep( id, stepsize ); 
}

inline
void WorldSetQuickStepNumIterations( dWorldID id, int num ) 
{ 
    dWorldSetQuickStepNumIterations( id, num ); 
}

inline
int WorldGetQuickStepNumIterations( dWorldID id ) 
{ 
    return dWorldGetQuickStepNumIterations( id ); 
}

/*
void dWorldSetAutoEnableDepthSF1(dWorldID, int autoEnableDepth);
int dWorldGetAutoEnableDepthSF1(dWorldID);
void dBodySetAutoDisableThresholdSF1(dBodyID, dReal autoDisableThreshold);
dReal dBodyGetAutoDisableThresholdSF1(dBodyID);
void dBodySetAutoDisableStepsSF1(dBodyID, int AutoDisableSteps);
int dBodyGetAutoDisableStepsSF1(dBodyID);
void dBodySetAutoDisableSF1(dBodyID, int doAutoDisable);
int dBodyGetAutoDisableSF1(dBodyID);
*/

//----------------------------------------------------------------------------
/*
    Body functions
*/

inline
dBodyID BodyCreate( dWorldID id ) { return dBodyCreate( id ); }

inline
void BodyDestroy( dBodyID id ) { dBodyDestroy( id ); }

inline
void BodySetPosition( dBodyID id, float x, float y, float z )
{
    dBodySetPosition( id, x, y, z );
}

inline
void BodySetPosition( dBodyID id, const vector3& pos )
{
    dBodySetPosition( id, pos.x, pos.y, pos.z );
}

inline
void BodySetRotation( dBodyID id, const matrix33& R )
{
    dMatrix3 t;
    nOpendeMarshal::matrix33_2_dMatrix3( R, t );
    dBodySetRotation( id, t );
}

inline
void BodySetRotation( dBodyID id, const matrix44& R )
{
    dMatrix3 t;
    nOpendeMarshal::matrix44_2_dMatrix3( R, t );
    dBodySetRotation( id, t );
}

inline
void BodySetQuaternion( dBodyID id, const quaternion& q )
{
    dQuaternion t;
    nOpendeMarshal::quat_2_dQuat( q, t );
    dBodySetQuaternion( id, t );
}

inline
void BodySetLinearVel( dBodyID id, float x, float y, float z )
{
    dBodySetLinearVel( id, x, y, z );
}

inline
void BodySetLinearVel( dBodyID id, const vector3& v )
{
    dBodySetLinearVel( id, v.x, v.y, v.z );
}

inline
void BodySetAngularVel( dBodyID id, float x, float y, float z )
{
    dBodySetAngularVel( id, x, y, z );
}

inline
void BodySetAngularVel( dBodyID id, const vector3& v )
{
    dBodySetAngularVel( id, v.x, v.y, v.z );
}

inline
vector3 BodyGetPosition( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dBodyGetPosition( id ) );
}

inline
matrix33 BodyGetRotation( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_matrix33( dBodyGetRotation( id ) );
}

inline
void BodyGetRotation( dBodyID id, matrix33& out )
{
    nOpendeMarshal::dRealPointer_2_matrix33( dBodyGetRotation( id ), out );
}

inline
quaternion BodyGetQuaternion( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_quat( dBodyGetQuaternion( id ) );
}

inline
vector3 BodyGetLinearVel( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dBodyGetLinearVel( id ) );
}

inline
vector3 BodyGetAngularVel( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dBodyGetAngularVel( id ) );
}

inline
void BodySetMass( dBodyID id, const dMass* mass ) { dBodySetMass( id, mass ); }

inline
void BodyGetMass( dBodyID id, dMass* mass ) { dBodyGetMass( id, mass ); }

inline
void BodyAddForce( dBodyID id, float fx, float fy, float fz )
{
    dBodyAddForce( id, fx, fy, fz );
}

inline
void BodyAddForce( dBodyID id, const vector3& f )
{
    dBodyAddForce( id, f.x, f.y, f.z );
}

inline
void BodyAddTorque( dBodyID id, float fx, float fy, float fz )
{
    dBodyAddTorque( id, fx, fy, fz );
}

inline
void BodyAddTorque( dBodyID id, const vector3& f )
{
    dBodyAddTorque( id, f.x, f.y, f.z );
}

inline
void BodyAddRelForce( dBodyID id, float fx, float fy, float fz )
{
    dBodyAddRelForce( id, fx, fy, fz );
}

inline
void BodyAddRelForce( dBodyID id, const vector3& f )
{
    dBodyAddRelForce( id, f.x, f.y, f.z );
}

inline
void BodyAddRelTorque( dBodyID id, float fx, float fy, float fz )
{
    dBodyAddRelTorque( id, fx, fy, fz );
}

inline
void BodyAddRelTorque( dBodyID id, const vector3& f )
{
    dBodyAddRelTorque( id, f.x, f.y, f.z );
}

inline
void BodyAddForceAtPos( dBodyID id, float fx, float fy, float fz,
                                    float px, float py, float pz )
{
    dBodyAddForceAtPos( id, fx, fy, fz, px, py, pz );
}

inline
void BodyAddForceAtPos( dBodyID id, const vector3& f, const vector3& p )
{
    dBodyAddForceAtPos( id, f.x, f.y, f.z, p.x, p.y, p.z );
}
                                        
inline
void BodyAddForceAtRelPos( dBodyID id, float fx, float fy, float fz,
                                       float px, float py, float pz)
{
    dBodyAddForceAtRelPos( id, fx, fy, fz, px, py, pz );
}

inline
void BodyAddForceAtRelPos( dBodyID id, const vector3& f, const vector3& p )
{
    dBodyAddForceAtRelPos( id, f.x, f.y, f.z, p.x, p.y, p.z );
}
                                        
inline
void BodyAddRelForceAtPos( dBodyID id, float fx, float fy, float fz,
                                       float px, float py, float pz)
{
    dBodyAddRelForceAtPos( id, fx, fy, fz, px, py, pz );
}

inline
void BodyAddRelForceAtPos( dBodyID id, const vector3& f, const vector3& p )
{
    dBodyAddRelForceAtPos( id, f.x, f.y, f.z, p.x, p.y, p.z );
}

inline
void BodyAddRelForceAtRelPos( dBodyID id, float fx, float fy, float fz,
                                          float px, float py, float pz )
{
    dBodyAddRelForceAtRelPos( id, fx, fy, fz, px, py, pz );
}

inline
void BodyAddRelForceAtRelPos( dBodyID id, const vector3& f, const vector3& p )
{
    dBodyAddRelForceAtRelPos( id, f.x, f.y, f.z, p.x, p.y, p.z );
}

inline
vector3 BodyGetForce( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dBodyGetForce( id ) );
}

inline
vector3 BodyGetTorque( dBodyID id )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dBodyGetTorque( id ) );
}

inline
void BodySetForce( dBodyID id, float x, float y, float z )
{
    dBodySetForce( id, x, y, z );
}

inline
void BodySetForce( dBodyID id, const vector3& f )
{
    dBodySetForce( id, f.x, f.y, f.z );
}

inline
void BodySetTorque( dBodyID id, float x, float y, float z )
{
    dBodySetTorque( id, x, y, z );
}

inline
void BodySetTorque( dBodyID id, const vector3& f )
{
    dBodySetTorque( id, f.x, f.y, f.z );
}

inline
void BodySetData( dBodyID id, void* data ) { dBodySetData( id, data ); }

inline
void* BodyGetData( dBodyID id ) { return dBodyGetData( id ); }

inline
void BodyEnable( dBodyID id ) { dBodyEnable( id ); }

inline
void BodyDisable( dBodyID id ) { dBodyDisable( id ); }

inline
bool BodyIsEnabled( dBodyID id ) { return dBodyIsEnabled( id ) == 1; }

inline
void BodySetFiniteRotationMode( dBodyID id, int mode )
{
    dBodySetFiniteRotationMode( id, mode );
}

inline
int BodyGetFiniteRotationMode( dBodyID id ) 
{ 
    return dBodyGetFiniteRotationMode( id );
}

inline
void BodySetFiniteRotationAxis( dBodyID id, float x, float y, float z )
{
    dBodySetFiniteRotationAxis( id, x, y, z );
}

inline
void BodySetFiniteRotationAxis( dBodyID id, const vector3& a )
{
    dBodySetFiniteRotationAxis( id, a.x, a.y, a.z );
}

inline
void BodyGetFiniteRotationAxis( dBodyID id, vector3& result )
{
    dVector3 t;
    dBodyGetFiniteRotationAxis( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
int BodyGetNumJoints( dBodyID id ) { return dBodyGetNumJoints( id ); }

inline
dJointID BodyGetJoint( dBodyID id, int index ) 
{ 
    return dBodyGetJoint( id, index );
}

inline
void BodySetGravityMode( dBodyID id, bool mode )
{
    dBodySetGravityMode( id, mode );
}

inline
bool BodyGetGravityMode( dBodyID id )
{
    return dBodyGetGravityMode( id ) == 1;
}

/*
    dBody utility functions
*/

inline
void BodyGetRelPointPos( dBodyID id, float px, float py, float pz, 
                         vector3& result )
{
    dVector3 t;
    dBodyGetRelPointPos( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetRelPointPos( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyGetRelPointPos( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetRelPointVel( dBodyID id, float px, float py, float pz,
                         vector3& result )
{
    dVector3 t;
    dBodyGetRelPointVel( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetRelPointVel( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyGetRelPointVel( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetPointVel( dBodyID id, float px, float py, float pz,
                      vector3& result )
{
    dVector3 t;
    dBodyGetPointVel( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetPointVel( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyGetPointVel( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetPosRelPoint( dBodyID id, float px, float py, float pz,
                         vector3& result )
{
    dVector3 t;
    dBodyGetPosRelPoint( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyGetPosRelPoint( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyGetPosRelPoint( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyVectorToWorld( dBodyID id, float px, float py, float pz,
                        vector3& result )
{
    dVector3 t;
    dBodyVectorToWorld( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyVectorToWorld( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyVectorToWorld( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyVectorFromWorld( dBodyID id, float px, float py, float pz,
                          vector3& result )
{
    dVector3 t;
    dBodyVectorFromWorld( id, px, py, pz, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void BodyVectorFromWorld( dBodyID id, const vector3& p, vector3& result )
{
    dVector3 t;
    dBodyVectorFromWorld( id, p.x, p.y, p.z, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}


//----------------------------------------------------------------------------
/*
    Joint functions
*/

inline
dJointID JointCreateBall( dWorldID world, dJointGroupID group )
{
    return dJointCreateBall( world, group );
}

inline
dJointID JointCreateHinge( dWorldID world, dJointGroupID group )
{
    return dJointCreateHinge( world, group );
}

inline
dJointID JointCreateSlider( dWorldID world, dJointGroupID group )
{
    return dJointCreateSlider( world, group );
}

inline
dJointID JointCreateContact( dWorldID world, dJointGroupID group, 
                             const dContact* contact )
{
    return dJointCreateContact( world, group, contact );
}

inline
dJointID JointCreateUniversal( dWorldID world, dJointGroupID group )
{
    return dJointCreateUniversal( world, group );
}

inline
dJointID JointCreateHinge2( dWorldID world, dJointGroupID group )
{
    return dJointCreateHinge2( world, group );
}

inline
dJointID JointCreateFixed( dWorldID world, dJointGroupID group )
{
    return dJointCreateFixed( world, group );
}

inline
dJointID JointCreateAMotor( dWorldID world, dJointGroupID group )
{
    return dJointCreateAMotor( world, group );
}

inline
void JointDestroy( dJointID id ) { dJointDestroy( id ); }

inline
dJointGroupID JointGroupCreate( int max_size ) { return dJointGroupCreate( max_size ); }

inline
void JointGroupDestroy( dJointGroupID id ) { dJointGroupDestroy( id ); }

inline
void JointGroupEmpty( dJointGroupID id ) { dJointGroupEmpty( id ); }

inline
void JointAttach( dJointID joint, dBodyID body1, dBodyID body2 )
{
    dJointAttach( joint, body1, body2 );
}

inline
void JointSetData( dJointID id, void* data ) { dJointSetData( id, data ); }

inline
void* JointGetData( dJointID id ) { return dJointGetData( id ); }

inline
int JointGetType( dJointID id ) { return dJointGetType( id ); }

inline
dBodyID JointGetBody( dJointID joint, int index )
{
    return dJointGetBody( joint, index );
}

inline
void JointSetFeedback( dJointID id, dJointFeedback* feedback )
{
    dJointSetFeedback( id, feedback );
}

inline
dJointFeedback* JointGetFeedback( dJointID id )
{
    return dJointGetFeedback( id );
}

inline
void JointGetFeedback( dJointID id, vector3& force1, vector3& torque1,
                       vector3& force2, vector3& torque2 )
{
    dJointFeedback* feedback = dJointGetFeedback( id );
    n_assert( feedback && "Feedback disabled!" );
    nOpendeMarshal::dVector3_2_vector3( feedback->f1, force1 );
    nOpendeMarshal::dVector3_2_vector3( feedback->t1, torque1 );
    nOpendeMarshal::dVector3_2_vector3( feedback->f2, force2 );
    nOpendeMarshal::dVector3_2_vector3( feedback->t2, torque2 );
}

inline
bool AreConnected( dBodyID body1, dBodyID body2 )
{
    return dAreConnected( body1, body2 ) == 1;
}

inline
bool AreConnectedExcluding( dBodyID body1, dBodyID body2, int joint_type )
{
    return dAreConnectedExcluding( body1, body2, joint_type ) == 1;
}

inline
void JointSetBallAnchor( dJointID id, float x, float y, float z )
{
    dJointSetBallAnchor( id, x, y, z );
}

inline
void JointSetBallAnchor( dJointID id, const vector3& a )
{
    dJointSetBallAnchor( id, a.x, a.y, a.z );
}

inline
void JointGetBallAnchor( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetBallAnchor( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetBallAnchor2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetBallAnchor2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointSetHingeAnchor( dJointID id, float x, float y, float z )
{
    dJointSetHingeAnchor( id, x, y, z );
}

inline
void JointSetHingeAnchor( dJointID id, const vector3& a )
{
    dJointSetHingeAnchor( id, a.x, a.y, a.z );
}

inline
void JointSetHingeAxis( dJointID id, float x, float y, float z )
{
    dJointSetHingeAxis( id, x, y, z );
}

inline
void JointSetHingeAxis( dJointID id, const vector3& a )
{
    dJointSetHingeAxis( id, a.x, a.y, a.z );
}

inline
void JointGetHingeAnchor( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHingeAnchor( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetHingeAnchor2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHingeAnchor2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetHingeAxis( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHingeAxis( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
float JointGetHingeAngle( dJointID id ) 
{ 
    return (float)dJointGetHingeAngle( id ); 
}

inline
float JointGetHingeAngleRate( dJointID id ) 
{ 
    return (float)dJointGetHingeAngleRate( id );
}

inline
void JointSetSliderAxis( dJointID id, float x, float y, float z )
{
    dJointSetSliderAxis( id, x, y, z );
}

inline
void JointSetSliderAxis( dJointID id, const vector3& a )
{
    dJointSetSliderAxis( id, a.x, a.y, a.z );
}

inline
void JointGetSliderAxis( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetSliderAxis( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
float JointGetSliderPosition( dJointID id )
{
    return (float)dJointGetSliderPosition( id );
}

inline
float JointGetSliderPositionRate( dJointID id )
{
    return (float)dJointGetSliderPositionRate( id );
}

inline
void JointSetUniversalAnchor( dJointID id, float x, float y, float z )
{
    dJointSetUniversalAnchor( id, x, y, z );
}

inline
void JointSetUniversalAnchor( dJointID id, const vector3& a )
{
    dJointSetUniversalAnchor( id, a.x, a.y, a.z );
}

inline
void JointSetUniversalAxis1( dJointID id, float x, float y, float z )
{
    dJointSetUniversalAxis1( id, x, y, z );
}

inline
void JointSetUniversalAxis1( dJointID id, const vector3& a )
{
    dJointSetUniversalAxis1( id, a.x, a.y, a.z );
}

inline
void JointSetUniversalAxis2( dJointID id, float x, float y, float z )
{
    dJointSetUniversalAxis2( id, x, y, z );
}

inline
void JointSetUniversalAxis2( dJointID id, const vector3& a )
{
    dJointSetUniversalAxis2( id, a.x, a.y, a.z );
}

inline
void JointGetUniversalAnchor( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetUniversalAnchor( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetUniversalAnchor2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetUniversalAnchor2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetUniversalAxis1( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetUniversalAxis1( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetUniversalAxis2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetUniversalAxis2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointSetHinge2Anchor( dJointID id, float x, float y, float z )
{
    dJointSetHinge2Anchor( id, x, y, z );
}

inline
void JointSetHinge2Anchor( dJointID id, const vector3& a )
{
    dJointSetHinge2Anchor( id, a.x, a.y, a.z );
}

inline
void JointSetHinge2Axis1( dJointID id, float x, float y, float z )
{
    dJointSetHinge2Axis1( id, x, y, z );
}

inline
void JointSetHinge2Axis1( dJointID id, const vector3& a )
{
    dJointSetHinge2Axis1( id, a.x, a.y, a.z );
}

inline
void JointSetHinge2Axis2( dJointID id, float x, float y, float z )
{
    dJointSetHinge2Axis2( id, x, y, z );
}

inline
void JointSetHinge2Axis2( dJointID id, const vector3& a )
{
    dJointSetHinge2Axis2( id, a.x, a.y, a.z );
}

inline
void JointGetHinge2Anchor( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHinge2Anchor( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetHinge2Anchor2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHinge2Anchor2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetHinge2Axis1( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHinge2Axis1( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
void JointGetHinge2Axis2( dJointID id, vector3& result )
{
    dVector3 t;
    dJointGetHinge2Axis2( id, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
float JointGetHinge2Angle1( dJointID id )
{
    return (float)dJointGetHinge2Angle1( id );
}

inline
float JointGetHinge2Angle1Rate( dJointID id )
{
    return (float)dJointGetHinge2Angle1Rate( id );
}

inline
float JointGetHinge2Angle2Rate( dJointID id )
{
    return (float)dJointGetHinge2Angle2Rate( id );
}

inline
void JointSetFixed( dJointID id ) { dJointSetFixed( id ); }

inline
void JointSetAMotorMode( dJointID id, int mode ) 
{ 
    dJointSetAMotorMode( id, mode );
}

inline
int JointGetAMotorMode( dJointID id ) { return dJointGetAMotorMode( id ); }

inline
void JointSetAMotorNumAxes( dJointID id, int num )
{
    dJointSetAMotorNumAxes( id, num );
}

inline
int JointGetAMotorNumAxes( dJointID id )
{
    return dJointGetAMotorNumAxes( id );
}

inline
void JointSetAMotorAxis( dJointID id, int anum, int rel,
                         float x, float y, float z )
{
    dJointSetAMotorAxis( id, anum, rel, x, y, z );
}

inline
void JointSetAMotorAxis( dJointID id, int anum, int rel, const vector3& a )
{
    dJointSetAMotorAxis( id, anum, rel, a.x, a.y, a.z );
}

inline
void JointGetAMotorAxis( dJointID id, int anum, vector3& result )
{
    dVector3 t;
    dJointGetAMotorAxis( id, anum, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
int JointGetAMotorAxisRel( dJointID id, int anum )
{
    return dJointGetAMotorAxisRel( id, anum );
}

inline
void JointSetAMotorAngle( dJointID id, int anum, float angle )
{
    dJointSetAMotorAngle( id, anum, angle );
}

inline
float JointGetAMotorAngle( dJointID id, int anum )
{
    return (float)dJointGetAMotorAngle( id, anum );
}

inline
float JointGetAMotorAngleRate( dJointID id, int anum )
{
    return (float)dJointGetAMotorAngleRate( id, anum );
}

inline
void JointSetHingeParam( dJointID id, int parameter, float value )
{
    dJointSetHingeParam( id, parameter, value );
}

inline
void JointSetSliderParam( dJointID id, int parameter, float value )
{
    dJointSetSliderParam( id, parameter, value );
}

inline
void JointSetHinge2Param( dJointID id, int parameter, float value )
{
    dJointSetHinge2Param( id, parameter, value );
}

inline
void JointSetAMotorParam( dJointID id, int parameter, float value )
{
    dJointSetAMotorParam( id, parameter, value );
}

inline
void JointSetUniversalParam( dJointID id, int parameter, float value )
{
    dJointSetUniversalParam( id, parameter, value );
}

inline
float JointGetHingeParam( dJointID id, int parameter )
{
    return (float)dJointGetHingeParam( id, parameter );
}

inline
float JointGetSliderParam( dJointID id, int parameter )
{
    return (float)dJointGetSliderParam( id, parameter );
}

inline
float JointGetHinge2Param( dJointID id, int parameter )
{
    return (float)dJointGetHinge2Param( id, parameter );
}

inline
float JointGetAMotorParam( dJointID id, int parameter )
{
    return (float)dJointGetAMotorParam( id, parameter );
}

inline
float JointGetUniversalParam( dJointID id, int parameter )
{
    return (float)dJointGetUniversalParam( id, parameter );
}

//----------------------------------------------------------------------------
/*
    Mass functions
*/

inline
void MassSetZero( dMass* mass ) { dMassSetZero( mass ); }

inline
void MassSetParameters( dMass* mass, float themass,
                        float cgx, float cgy, float cgz,
                        float I11, float I22, float I33,
                        float I12, float I13, float I23)
{
    dMassSetParameters( mass, themass, cgx, cgy, cgz,
                        I11, I22, I33, I12, I13, I23 );
}

inline
void MassSetSphere( dMass* mass, float density, float radius )
{
    dMassSetSphere( mass, density, radius );
}

inline
void MassSetSphereTotal( dMass* mass, float total_mass, float radius )
{
    dMassSetSphereTotal( mass, total_mass, radius );
}

inline
void MassSetCappedCylinder( dMass* mass, float density, int direction,
                            float radius, float length )
{
    dMassSetCappedCylinder( mass, density, direction, radius, length );
}

inline
void MassSetCappedCylinderTotal( dMass* mass, float total_mass, int direction,
                                 float radius, float length )
{
    dMassSetCappedCylinderTotal( mass, total_mass, direction, radius, length );
}

inline
void MassSetCylinder( dMass* mass, float density, int direction,
                      float radius, float length )
{
    dMassSetCylinder( mass, density, direction, radius, length );
}

inline
void MassSetCylinderTotal( dMass* mass, float total_mass, int direction,
                           float radius, float length )
{
    dMassSetCylinderTotal( mass, total_mass, direction, radius, length );
}

inline
void MassSetBox( dMass* mass, float density, float lx, float ly, float lz )
{
    dMassSetBox( mass, density, lx, ly, lz );
}

inline
void MassSetBoxTotal( dMass* mass, float total_mass, 
                      float lx, float ly, float lz )
{
    dMassSetBoxTotal( mass, total_mass, lx, ly, lz );
}

inline
void MassAdjust( dMass* mass, float newmass ) { dMassAdjust( mass, newmass ); }

inline
void MassTranslate( dMass* mass, float x, float y, float z )
{
    dMassTranslate( mass, x, y, z );
}

inline
void MassTranslate( dMass* mass, const vector3& t )
{
    dMassTranslate( mass, t.x, t.y, t.z );
}

inline
void MassRotate( dMass* mass, const matrix33& R )
{
    dMatrix3 t;
    nOpendeMarshal::matrix33_2_dMatrix3( R, t );
    dMassRotate( mass, t );
}

inline
void MassAdd( dMass* a, const dMass* b ) { dMassAdd( a, b ); }

//----------------------------------------------------------------------------
/*
    Geom functions
*/

inline
void GeomDestroy( dGeomID id ) { dGeomDestroy( id ); }

inline
void GeomSetData( dGeomID id, void* data ) { dGeomSetData( id, data ); }

inline
void* GeomGetData( dGeomID id ) { return dGeomGetData( id ); }

inline
void GeomSetBody( dGeomID g, dBodyID b ) { dGeomSetBody( g, b ); }

inline
dBodyID GeomGetBody( dGeomID g ) { return dGeomGetBody( g ); }

inline
void GeomSetPosition( dGeomID g, float x, float y, float z )
{
    dGeomSetPosition( g, x, y, z );
}

inline
void GeomSetPosition( dGeomID g, const vector3& p )
{
    dGeomSetPosition( g, p.x, p.y, p.z );
}

inline
void GeomSetRotation( dGeomID g, const matrix33& R )
{
    dMatrix3 t;
    nOpendeMarshal::matrix33_2_dMatrix3( R, t );
    dGeomSetRotation( g, t );
}

inline
void GeomSetQuaternion( dGeomID g, const quaternion& q )
{
    dQuaternion t;
    nOpendeMarshal::quat_2_dQuat( q, t );
    dGeomSetQuaternion( g, t );
}

inline
vector3 GeomGetPosition( dGeomID g )
{
    return nOpendeMarshal::dRealPointer_2_vector3( dGeomGetPosition( g ) );
}

inline
matrix33 GeomGetRotation( dGeomID g )
{
    return nOpendeMarshal::dRealPointer_2_matrix33( dGeomGetRotation( g ) );
}

inline
void GeomGetRotation( dGeomID g, matrix33& out )
{
    nOpendeMarshal::dRealPointer_2_matrix33( dGeomGetRotation( g ), out );
}

inline
void GeomGetQuaternion( dGeomID g, quaternion& result )
{
    dQuaternion t;
    dGeomGetQuaternion( g, t );
    nOpendeMarshal::dQuat_2_quat( t, result );
}

inline
void GeomGetAABB( dGeomID g, float aabb[6] )
{
    dReal t[6];
    dGeomGetAABB( g, t );
    aabb[0] = t[0];
    aabb[1] = t[1];
    aabb[2] = t[2];
    aabb[3] = t[3];
    aabb[4] = t[4];
    aabb[5] = t[5];
}

inline
bool GeomIsSpace( dGeomID g ) { return dGeomIsSpace( g ) == 1; }

inline
dSpaceID GeomGetSpace( dGeomID g ) { return dGeomGetSpace( g ); }

inline
int GeomGetClass( dGeomID g ) { return dGeomGetClass( g ); }

inline
void GeomSetCategoryBits( dGeomID g, ulong bits )
{
    dGeomSetCategoryBits( g, bits );
}

inline
void GeomSetCollideBits( dGeomID g, ulong bits )
{
    dGeomSetCollideBits( g, bits );
}

inline
ulong GeomGetCategoryBits( dGeomID g )
{
    return dGeomGetCategoryBits( g );
}

inline
ulong GeomGetCollideBits( dGeomID g )
{
    return dGeomGetCollideBits( g );
}

inline
void GeomEnable( dGeomID g ) { dGeomEnable( g ); }

inline
void GeomDisable( dGeomID g ) { dGeomDisable( g ); }

inline
bool GeomIsEnabled( dGeomID g ) { return dGeomIsEnabled( g ) == 1; }

//----------------------------------------------------------------------------
/*
    Collision detection functions
*/

inline
int Collide( dGeomID o1, dGeomID o2, int flags, 
             dContactGeom* contact, int skip )
{
    return dCollide( o1, o2, flags, contact, skip );
}

inline
void SpaceCollide( dSpaceID space, void* data, dNearCallback* callback )
{
    dSpaceCollide( space, data, callback );
}

inline
void SpaceCollide2( dGeomID o1, dGeomID o2, void* data, 
                    dNearCallback* callback )
{
    dSpaceCollide2( o1, o2, data, callback );
}

//----------------------------------------------------------------------------
/*
    Space functions
*/

inline
dSpaceID SimpleSpaceCreate( dSpaceID space )
{
    return dSimpleSpaceCreate( space );
}

inline
dSpaceID HashSpaceCreate( dSpaceID space )
{
    return dHashSpaceCreate( space );
}

inline
dSpaceID QuadTreeSpaceCreate( dSpaceID space, const vector3& center,
                              const vector3& extents, int depth )
{
    dVector3 dCenter, dExtents;
    nOpendeMarshal::vector3_2_dVector3( center, dCenter );
    nOpendeMarshal::vector3_2_dVector3( extents, dExtents );
    return dQuadTreeSpaceCreate( space, dCenter, dExtents, depth );
}

inline
void SpaceDestroy( dSpaceID space ) { dSpaceDestroy( space ); }

inline
void HashSpaceSetLevels( dSpaceID space, int minlevel, int maxlevel )
{
    dHashSpaceSetLevels( space, minlevel, maxlevel );
}

inline
void HashSpaceGetLevels( dSpaceID space, int* minlevel, int* maxlevel )
{
    dHashSpaceGetLevels( space, minlevel, maxlevel );
}

inline
void SpaceSetCleanup( dSpaceID space, int mode ) 
{ 
    dSpaceSetCleanup( space, mode );
}

inline
int SpaceGetCleanup( dSpaceID space ) { return dSpaceGetCleanup( space ); }

inline
void SpaceAdd( dSpaceID space, dGeomID g ) { dSpaceAdd( space, g ); }

inline
void SpaceRemove( dSpaceID space, dGeomID g ) { dSpaceRemove( space, g ); }

inline
bool SpaceQuery( dSpaceID space, dGeomID g )
{
    return dSpaceQuery( space, g ) == 1;
}

inline
int SpaceGetNumGeoms( dSpaceID space ) { return dSpaceGetNumGeoms( space ); }

inline
dGeomID SpaceGetGeom( dSpaceID space, int i )
{
    return dSpaceGetGeom( space, i );
}

//----------------------------------------------------------------------------
/*
    More Geom functions
*/

inline
dGeomID CreateSphere( dSpaceID space, float radius )
{
    return dCreateSphere( space, radius );
}

inline
void GeomSphereSetRadius( dGeomID sphere, float radius )
{
    dGeomSphereSetRadius( sphere, radius );
}

inline
float GeomSphereGetRadius( dGeomID sphere )
{
    return (float)dGeomSphereGetRadius( sphere );
}

inline
float GeomSpherePointDepth( dGeomID sphere, float x, float y, float z )
{
    return (float)dGeomSpherePointDepth( sphere, x, y, z );
}

inline
float GeomSpherePointDepth( dGeomID sphere, const vector3& p )
{
    return (float)dGeomSpherePointDepth( sphere, p.x, p.y, p.z );
}


inline
dGeomID CreateBox( dSpaceID space, float lx, float ly, float lz )
{
    return dCreateBox( space, lx, ly, lz );
}

inline
void GeomBoxSetLengths( dGeomID box, float lx, float ly, float lz )
{
    dGeomBoxSetLengths( box, lx, ly, lz );
}

inline
void GeomBoxGetLengths( dGeomID box, vector3& result )
{
    dVector3 t;
    dGeomBoxGetLengths( box, t );
    nOpendeMarshal::dVector3_2_vector3( t, result );
}

inline
float GeomBoxPointDepth( dGeomID box, float x, float y, float z )
{
    return (float)dGeomBoxPointDepth( box, x, y, z );
}

inline
float GeomBoxPointDepth( dGeomID box, const vector3& p )
{
    return (float)dGeomBoxPointDepth( box, p.x, p.y, p.z );
}


inline
dGeomID CreatePlane( dSpaceID space, float a, float b, float c, float d )
{
    return dCreatePlane( space, a, b, c, d );
}

inline
void GeomPlaneSetParams( dGeomID g, float a, float b, float c, float d )
{
    dGeomPlaneSetParams( g, a, b, c, d );
}

inline
void GeomPlaneGetParams( dGeomID g, vector4& result )
{
    dVector4 t;
    dGeomPlaneGetParams( g, t );
    nOpendeMarshal::dVector4_2_vector4( t, result );
}

inline
float GeomPlanePointDepth( dGeomID g, float x, float y, float z )
{
    return (float)dGeomPlanePointDepth( g, x, y, z );
}

inline
float GeomPlanePointDepth( dGeomID g, const vector3& p )
{
    return (float)dGeomPlanePointDepth( g, p.x, p.y, p.z );
}


inline
dGeomID CreateCCylinder( dSpaceID space, float radius, float length )
{
    return dCreateCCylinder( space, radius, length );
}

inline
void GeomCCylinderSetParams( dGeomID ccylinder, float radius, float length )
{
    dGeomCCylinderSetParams( ccylinder, radius, length );
}

inline
void GeomCCylinderGetParams( dGeomID ccylinder, float* radius, float* length )
{
    dGeomCCylinderGetParams( ccylinder, radius, length );
}

inline
float GeomCCylinderPointDepth( dGeomID ccylinder, float x, float y, float z )
{
    return (float)dGeomCCylinderPointDepth( ccylinder, x, y, z );
}

inline
float GeomCCylinderPointDepth( dGeomID ccylinder, const vector3& p )
{
    return (float)dGeomCCylinderPointDepth( ccylinder, p.x, p.y, p.z );
}


inline
dGeomID CreateRay( dSpaceID space, float length )
{
    return dCreateRay( space, length );
}

inline
void GeomRaySetLength( dGeomID ray, float length )
{
    dGeomRaySetLength( ray, length );
}

inline
float GeomRayGetLength( dGeomID ray )
{
    return (float)dGeomRayGetLength( ray );
}

inline
void GeomRaySet( dGeomID ray, float px, float py, float pz,
                              float dx, float dy, float dz )
{
    dGeomRaySet( ray, px, py, pz, dx, dy, dz );
}

inline
void GeomRaySet( dGeomID ray, const vector3& pos, const vector3& dir )
{
    dGeomRaySet( ray, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z );
}

inline
void GeomRayGet( dGeomID ray, vector3& start, vector3& dir )
{
    dVector3 odeStart, odeDir;
    dGeomRayGet( ray, odeStart, odeDir );
    nOpendeMarshal::dVector3_2_vector3( odeStart, start );
    nOpendeMarshal::dVector3_2_vector3( odeDir, dir );
}

inline
void GeomRaySetParams( dGeomID ray, int firstContact, int backfaceCull )
{
    dGeomRaySetParams( ray, firstContact, backfaceCull );
}

inline
void GeomRayGetParams( dGeomID ray, int* firstContact, int* backfaceCull )
{
    dGeomRayGetParams( ray, firstContact, backfaceCull );
}

inline
void GeomRaySetClosestHit( dGeomID ray, bool closestHit )
{
    dGeomRaySetClosestHit( ray, closestHit ? 1 : 0 );
}

inline
bool GeomRayGetClosestHit( dGeomID ray )
{
    return dGeomRayGetClosestHit( ray ) == 1;
}

//----------------------------------------------------------------------------
/*
    dTriMesh functions
*/

inline
dTriMeshDataID GeomTriMeshDataCreate()
{
    return dGeomTriMeshDataCreate();
}

inline
void GeomTriMeshDataDestroy( dTriMeshDataID g )
{
    dGeomTriMeshDataDestroy( g );
}

inline
void GeomTriMeshDataBuildSingle( dTriMeshDataID g, const void* vertices,
                                 int vertexStride, int vertexCount, 
                                 const void* indices, int indexCount, 
                                 int triStride )
{
    dGeomTriMeshDataBuildSingle( g, vertices, vertexStride, vertexCount, 
                                 indices, indexCount, triStride );
}

inline
void GeomTriMeshDataBuildDouble( dTriMeshDataID g, const void* vertices,
                                 int vertexStride, int vertexCount, 
                                 const void* indices, int indexCount, 
                                 int triStride )
{
    dGeomTriMeshDataBuildDouble( g, vertices, vertexStride, vertexCount, 
                                 indices, indexCount, triStride );
}

inline
void GeomTriMeshDataBuildSimple( dTriMeshDataID g, const dReal* vertices, 
                                 int vertexCount, const int* indices, 
                                 int indexCount )
{
    dGeomTriMeshDataBuildSimple( g, vertices, vertexCount, indices, indexCount );
}

inline
void GeomTriMeshSetCallback( dGeomID g, dTriCallback* callback )
{
    dGeomTriMeshSetCallback( g, callback );
}

inline
dTriCallback* GeomTriMeshGetCallback( dGeomID g )
{
    return dGeomTriMeshGetCallback( g );
}

inline
void GeomTriMeshSetArrayCallback( dGeomID g, dTriArrayCallback* arrayCallback )
{
    dGeomTriMeshSetArrayCallback( g, arrayCallback );
}

inline
dTriArrayCallback* GeomTriMeshGetArrayCallback( dGeomID g )
{
    return dGeomTriMeshGetArrayCallback( g );
}

inline
void GeomTriMeshSetRayCallback( dGeomID g, dTriRayCallback* callback )
{
    dGeomTriMeshSetRayCallback( g, callback );
}

inline
dTriRayCallback* GeomTriMeshGetRayCallback( dGeomID g )
{
    return dGeomTriMeshGetRayCallback( g );
}

inline
dGeomID CreateTriMesh( dSpaceID space, dTriMeshDataID data, 
                       dTriCallback* callback, dTriArrayCallback* arrayCallback,
                       dTriRayCallback* rayCallback )
{
    return dCreateTriMesh( space, data, callback, arrayCallback, rayCallback );
}

inline
void GeomTriMeshSetData( dGeomID g, dTriMeshDataID data )
{
    dGeomTriMeshSetData( g, data );
}

inline
void GeomTriMeshEnableTC( dGeomID g, int geomClass, bool enable )
{
    dGeomTriMeshEnableTC( g, geomClass, enable ? 1 : 0 );
}

inline
bool GeomTriMeshIsTCEnabled( dGeomID g, int geomClass )
{
    return dGeomTriMeshIsTCEnabled( g, geomClass ) == 1;
}

inline
void GeomTriMeshClearTCCache( dGeomID g )
{
    dGeomTriMeshClearTCCache( g );
}

inline
void GeomTriMeshGetTriangle( dGeomID g, int index, 
                             vector3& v0, vector3& v1, vector3& v2 )
{
    dVector3 v[3];
    dGeomTriMeshGetTriangle( g, index, &(v[0]), &(v[1]), &(v[2]) );
    nOpendeMarshal::dVector3_2_vector3( v[0], v0 );
    nOpendeMarshal::dVector3_2_vector3( v[1], v1 );
    nOpendeMarshal::dVector3_2_vector3( v[2], v2 );
}

inline
void GeomTriMeshGetPoint( dGeomID g, int index, 
                          float u, float v, vector3& out )
{
    dVector3 t;
    dGeomTriMeshGetPoint( g, index, u, v, t );
    nOpendeMarshal::dVector3_2_vector3( t, out );
}

//----------------------------------------------------------------------------
/*
    TransformGeom functions
*/

inline
dGeomID CreateGeomTransform( dSpaceID space )
{
    return dCreateGeomTransform( space );
}

inline
void GeomTransformSetGeom( dGeomID g, dGeomID obj )
{
    dGeomTransformSetGeom( g, obj );
}

inline
dGeomID GeomTransformGetGeom( dGeomID g )
{
    return dGeomTransformGetGeom( g );
}

inline
void GeomTransformSetCleanup( dGeomID g, int mode )
{
    dGeomTransformSetCleanup( g, mode );
}

inline
int GeomTransformGetCleanup( dGeomID g )
{
    return dGeomTransformGetCleanup( g );
}

inline
void GeomTransformSetInfo( dGeomID g, int mode )
{
    dGeomTransformSetInfo( g, mode );
}

inline
int GeomTransformGetInfo( dGeomID g )
{
    return dGeomTransformGetInfo( g );
}

} // namespace nOpende

//----------------------------------------------------------------------------
#endif // N_OPENDELAYER_H
