//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendebody.h"

nNebulaScriptClass(nOpendeBody, "nroot");

#include "opende/nopendeworld.h"

//----------------------------------------------------------------------------
/**
*/
nOpendeBody::nOpendeBody() :
    id(0)
{
    //
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::Create( const char* world )
{
    this->ref_World = world;
    n_assert( this->ref_World.isvalid() );
    this->id = nOpende::BodyCreate( this->ref_World->id );
}

//----------------------------------------------------------------------------
/**
*/
nOpendeBody::~nOpendeBody()
{
    if ( this->id )
    {
        nOpende::BodyDestroy( this->id );
        this->id = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetPosition( const vector3& p )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetPosition( this->id, p );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetRotation( const matrix33& R )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetRotation( this->id, R );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetQuaternion( const quaternion& q )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetQuaternion( this->id, q );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetLinearVel( const vector3& v )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetLinearVel( this->id, v );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetAngularVel( const vector3& v )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetAngularVel( this->id, v );
}

//----------------------------------------------------------------------------
/**
*/
vector3 nOpendeBody::GetPosition()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetPosition( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetRotation( matrix33& R )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetRotation( this->id, R );
}

//----------------------------------------------------------------------------
/**
*/
quaternion nOpendeBody::GetQuaternion()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetQuaternion( this->id );
}

//----------------------------------------------------------------------------
/**
*/
vector3 nOpendeBody::GetLinearVel()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetLinearVel( this->id );
}

//----------------------------------------------------------------------------
/**
*/
vector3 nOpendeBody::GetAngularVel()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetAngularVel( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetMass( const dMass* m )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetMass( this->id, m );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetMass( dMass* m )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetMass( this->id, m );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddForce( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddForce( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddTorque( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddTorque( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddRelForce( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddRelForce( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddRelTorque( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddRelTorque( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddForceAtPos( const vector3& f, const vector3& p )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddForceAtPos( this->id, f, p );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddForceAtRelPos( const vector3& f, const vector3& p )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddForceAtRelPos( this->id, f, p );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddRelForceAtPos( const vector3& f, const vector3& p )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddRelForceAtPos( this->id, f, p );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::AddRelForceAtRelPos( const vector3& f, const vector3& p )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyAddRelForceAtRelPos( this->id, f, p );
}

//----------------------------------------------------------------------------
/**
*/
vector3 nOpendeBody::GetForce()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetForce( this->id );
}

//----------------------------------------------------------------------------
/**
*/
vector3 nOpendeBody::GetTorque()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetTorque( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetForce( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetForce( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetTorque( const vector3& f )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetTorque( this->id, f );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::Enable()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyEnable( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::Disable()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyDisable( this->id );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeBody::IsEnabled()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyIsEnabled( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetFiniteRotationMode( int mode )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetFiniteRotationMode( this->id, mode );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeBody::GetFiniteRotationMode()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetFiniteRotationMode( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetFiniteRotationAxis( const vector3& a )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetFiniteRotationAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetFiniteRotationAxis( vector3& a )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetFiniteRotationAxis( this->id, a );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeBody::GetNumJoints()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetNumJoints( this->id );
}

//----------------------------------------------------------------------------
/**
*/
dJointID nOpendeBody::GetJoint( int index )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetJoint( this->id, index );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetGravityMode( bool mode )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodySetGravityMode( this->id, mode );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeBody::GetGravityMode()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::BodyGetGravityMode( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetRelPointPos( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetRelPointPos( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetRelPointVel( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetRelPointVel( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetPointVel( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetPointVel( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::GetPosRelPoint( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyGetPosRelPoint( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::VectorToWorld( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyVectorToWorld( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::VectorFromWorld( const vector3& p, vector3& result )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nOpende::BodyVectorFromWorld( this->id, p, result );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeBody::IsConnectedTo( dBodyID body2 )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::AreConnected( this->id, body2 );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeBody::IsConnectedToExcluding( dBodyID body2, int joint_type )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    return nOpende::AreConnectedExcluding( this->id, body2, joint_type );
}

//----------------------------------------------------------------------------
/**
    @brief Set all mass parameters to zero.
*/
void nOpendeBody::ResetMass()
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::BodyGetMass( this->id, &mass );
    nOpende::MassSetZero( &mass );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Set the body mass parameters.
*/
void nOpendeBody::SetMassParams( float theMass, const vector3& cg,
                                 float I11, float I22, float I33,
                                 float I12, float I13, float I23 )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetParameters( &mass, theMass, cg.x, cg.y, cg.z,
                                I11, I22, I33, I12, I13, I23 );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetSphereMass( float density, float radius )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetSphere( &mass, density, radius );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetSphereMassTotal( float totalMass, float radius )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetSphereTotal( &mass, totalMass, radius );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Set the body mass to represent a capped cylinder.
*/
void nOpendeBody::SetCapsuleMass( float density, int direction,
                                  float radius, float length )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetCappedCylinder( &mass, density, direction, radius,
                                    length );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetCapsuleMassTotal( float totalMass, int direction,
                                       float radius, float length )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetCappedCylinderTotal( &mass, totalMass, direction, radius,
                                         length );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetCylinderMass( float density, int direction, float radius,
                                   float length )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetCylinder( &mass, density, direction, radius, length );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetCylinderMassTotal( float totalMass, int direction,
                                        float radius, float length )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetCylinderTotal( &mass, totalMass, direction, radius,
                                   length );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetBoxMass( float density, float lx, float ly, float lz )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetBox( &mass, density, lx, ly, lz );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeBody::SetBoxMassTotal( float totalMass,
                                   float lx, float ly, float lz )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::MassSetBoxTotal( &mass, totalMass, lx, ly, lz );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Equivalent to calling dMassAdjust on the body's mass.
*/
void nOpendeBody::AdjustMass( float newMass )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::BodyGetMass( this->id, &mass );
    nOpende::MassAdjust( &mass, newMass );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Equivalent to calling dMassTranslate on the body's mass.
*/
void nOpendeBody::TranslateMass( const vector3& t )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::BodyGetMass( this->id, &mass );
    nOpende::MassTranslate( &mass, t );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Equivalent to calling dMassRotate on the body's mass.
*/
void nOpendeBody::RotateMass( const matrix33& R )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    dMass mass;
    nOpende::BodyGetMass( this->id, &mass );
    nOpende::MassRotate( &mass, R );
    nOpende::BodySetMass( this->id, &mass );
}

//----------------------------------------------------------------------------
/**
    @brief Add the mass of another body to this body.
    @param otherBody The body whose mass should be added to this body.
*/
void nOpendeBody::AddMassOf( const char* otherBody )
{
    n_assert( this->id && "nOpendeBody::id not valid!" );
    nRoot* temp = this->kernelServer->Lookup( otherBody );
    n_assert( temp->IsA( this->kernelServer->FindClass( "nopendebody" ) ) );
    dMass myMass, otherMass;
    nOpende::BodyGetMass( this->id, &myMass );
    nOpende::BodyGetMass( ((nOpendeBody*)temp)->id, &otherMass );
    nOpende::MassAdd( &myMass, &otherMass );
    nOpende::BodySetMass( this->id, &myMass );
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
