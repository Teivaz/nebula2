//----------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//----------------------------------------------------------------------------
#include "opende/nopendejoint.h"

nNebulaScriptClass(nOpendeJoint, "nroot");

#include "opende/nopendeworld.h"
#include "opende/nopendebody.h"

const int nOpendeJoint::NUM_JOINT_PARAMS = 11;

const char* nOpendeJoint::JOINT_PARAM_NAMES[] = {
            "lostop", "histop", "vel", "fmax",
            "fudgefactor", "bounce", "cfm",
            "stoperp", "stopcfm",
            "suspensionerp", "suspensioncfm"
        };

//----------------------------------------------------------------------------
/**
*/
nOpendeJoint::nOpendeJoint() :
    id(0),
    firstBodyName("none"), secondBodyName("none")
{
    //
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::Create( const char* world )
{
    n_assert( 0 == this->id );
    this->ref_World = world;
    n_assert( this->ref_World.isvalid() );
    // joint will be created in subclass
}

//----------------------------------------------------------------------------
/**
*/
nOpendeJoint::~nOpendeJoint()
{
    if ( this->id )
    {
        nOpende::JointDestroy( this->id );
        this->id = 0;
    }
}

//----------------------------------------------------------------------------
/**
    @brief Attach the joint to the bodies.

    Note that using this method the connection will not be persisted, if
    you want the connection to be persistent use the string based AttachTo().
*/
void nOpendeJoint::AttachTo( dBodyID body1, dBodyID body2 )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointAttach( this->id, body1, body2 );
    this->firstBodyName = "none";
    this->secondBodyName = "none";
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::AttachTo( const char* body1, const char* body2 )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );

    dBodyID id1 = 0, id2 = 0;
    nRoot* temp;

    if ( strcmp( body1, "none" ) != 0 )
    {
        temp = this->kernelServer->Lookup( body1 );
        n_assert( temp &&
                  temp->IsA( this->kernelServer->FindClass( "nopendebody" ) ) &&
                  "body1 in call to nOpendeJoint::Attach() is invalid!" );
        id1 = ((nOpendeBody*)temp)->id;
    }

    this->firstBodyName = body1;

    if ( strcmp( body2, "none" ) != 0 )
    {
        temp = this->kernelServer->Lookup( body2 );
        n_assert( temp &&
                  temp->IsA( this->kernelServer->FindClass( "nopendebody" ) ) &&
                  "body2 in call to nOpendeJoint::Attach() is invalid!" );
        id2 = ((nOpendeBody*)temp)->id;
    }

    this->secondBodyName = body2;

    nOpende::JointAttach( this->id, id1, id2 );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::SetData( void* data )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetData( this->id, data );
}

//----------------------------------------------------------------------------
/**
*/
void* nOpendeJoint::GetData()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetData( this->id );
}

//----------------------------------------------------------------------------
/**
*/
int nOpendeJoint::GetJointType()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetType( this->id );
}

//----------------------------------------------------------------------------
/**
*/
const char* nOpendeJoint::GetJointTypeName()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );

    switch ( nOpende::JointGetType( this->id ) )
    {
        case dJointTypeBall:
            return "ball";
        case dJointTypeHinge:
            return "hinge";
        case dJointTypeSlider:
            return "slider";
        case dJointTypeContact:
            return "contact";
        case dJointTypeUniversal:
            return "universal";
        case dJointTypeHinge2:
            return "hinge2";
        case dJointTypeFixed:
            return "fixed";
        case dJointTypeAMotor:
            return "amotor";
        default:
            return "uknown";
    }
}

//----------------------------------------------------------------------------
/**
*/
dBodyID nOpendeJoint::GetBody( int index )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetBody( this->id, index );
}

//----------------------------------------------------------------------------
/**
*/
const char* nOpendeJoint::GetBodyName( int index )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    n_assert( (0 == index) || (1 == index) );
    if ( 0 == index )
        return this->firstBodyName.Get();
    else
        return this->secondBodyName.Get();
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::SetFeedback( dJointFeedback* feed )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointSetFeedback( this->id, feed );
}

//----------------------------------------------------------------------------
/**
*/
dJointFeedback* nOpendeJoint::GetFeedback()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return nOpende::JointGetFeedback( this->id );
}

//----------------------------------------------------------------------------
/**
*/
bool nOpendeJoint::IsFeedbackEnabled()
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    return 0 != nOpende::JointGetFeedback( this->id );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::GetFeedback( vector3& force1, vector3& torque1,
                                vector3& force2, vector3& torque2 )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    nOpende::JointGetFeedback( this->id, force1, torque1, force2, torque2 );
}

//----------------------------------------------------------------------------
/**
*/
void nOpendeJoint::SetParam( int param, float value )
{
    // empty
    n_assert( "Pure virtual method nOpendeJoint::SetParam() called!" );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeJoint::GetParam( int param )
{
    // empty
    n_assert( "Pure virtual method nOpendeJoint::GetParam() called!" );
    return 0.0f;
}

//----------------------------------------------------------------------------
/**
    @brief Set a joint parameter.
    @param axis The axis for which to set the parameter, can be 0, 1 or 2.
*/
void nOpendeJoint::SetParam2( const char* param, int axis, float value )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    n_assert( (0 <= axis) && (axis < 3) );
    int offset = dParamGroup * axis;

    if ( strcmp( param, "lostop" ) == 0 )
        this->SetParam( dParamLoStop + offset, value );
    else if ( strcmp( param, "histop" ) == 0 )
        this->SetParam( dParamHiStop + offset, value );
    else if ( strcmp( param, "vel" ) == 0 )
        this->SetParam( dParamVel + offset, value );
    else if ( strcmp( param, "fmax" ) == 0 )
        this->SetParam( dParamFMax + offset, value );
    else if ( strcmp( param, "fudgefactor" ) == 0 )
        this->SetParam( dParamFudgeFactor + offset, value );
    else if ( strcmp( param, "bounce" ) == 0 )
        this->SetParam( dParamBounce + offset, value );
    else if ( strcmp( param, "cfm" ) == 0 )
        this->SetParam( dParamCFM + offset, value );
    else if ( strcmp( param, "stoperp" ) == 0 )
        this->SetParam( dParamStopERP + offset, value );
    else if ( strcmp( param, "stopcfm" ) == 0 )
        this->SetParam( dParamStopCFM + offset, value );
    else if ( strcmp( param, "suspensionerp" ) == 0 )
        this->SetParam( dParamSuspensionERP + offset, value );
    else if ( strcmp( param, "suspensioncfm" ) == 0 )
        this->SetParam( dParamSuspensionCFM + offset, value );
    else
        n_error( "nOpendeJoint::SetParam(): Unknown param %s!", param );
}

//----------------------------------------------------------------------------
/**
*/
float nOpendeJoint::GetParam2( const char* param, int axis )
{
    n_assert( this->id && "nOpendeJoint::id not valid!" );
    n_assert( (0 <= axis) && (axis < 3) );
    int offset = dParamGroup * axis;

    if ( strcmp( param, "lostop" ) == 0 )
        return this->GetParam( dParamLoStop + offset );
    else if ( strcmp( param, "histop" ) == 0 )
        return this->GetParam( dParamHiStop + offset );
    else if ( strcmp( param, "vel" ) == 0 )
        return this->GetParam( dParamVel + offset );
    else if ( strcmp( param, "fmax" ) == 0 )
        return this->GetParam( dParamFMax + offset );
    else if ( strcmp( param, "fudgefactor" ) == 0 )
        return this->GetParam( dParamFudgeFactor + offset );
    else if ( strcmp( param, "bounce" ) == 0 )
        return this->GetParam( dParamBounce + offset );
    else if ( strcmp( param, "cfm" ) == 0 )
        return this->GetParam( dParamCFM + offset );
    else if ( strcmp( param, "stoperp" ) == 0 )
        return this->GetParam( dParamStopERP + offset );
    else if ( strcmp( param, "stopcfm" ) == 0 )
        return this->GetParam( dParamStopCFM + offset );
    else if ( strcmp( param, "suspensionerp" ) == 0 )
        return this->GetParam( dParamSuspensionERP + offset );
    else if ( strcmp( param, "suspensioncfm" ) == 0 )
        return this->GetParam( dParamSuspensionCFM + offset );
    else
        n_error( "nOpendeJoint::GetParam(): Unknown param %s!", param );

    return 0.0f;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
