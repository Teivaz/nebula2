//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeamotorjoint.h"
#include "kernel/npersistserver.h"

static void n_SetMode( void* slf, nCmd* cmd );
static void n_GetMode( void* slf, nCmd* cmd );
static void n_SetNumAxes( void* slf, nCmd* cmd );
static void n_GetNumAxes( void* slf, nCmd* cmd );
static void n_SetAxis( void* slf, nCmd* cmd );
static void n_GetAxis( void* slf, nCmd* cmd );
static void n_GetAxisRel( void* slf, nCmd* cmd );
static void n_SetAngle( void* slf, nCmd* cmd );
static void n_GetAngle( void* slf, nCmd* cmd );
static void n_GetAngleRate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeamotorjoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some angular motor joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetMode_s",       'SM__', n_SetMode );
    clazz->AddCmd( "s_GetMode_v",       'GM__', n_GetMode );
    clazz->AddCmd( "v_SetNumAxes_i",    'SNA_', n_SetNumAxes );
    clazz->AddCmd( "i_GetNumAxes_v",    'GNA_', n_GetNumAxes );
    clazz->AddCmd( "v_SetAxis_iifff",   'SA__', n_SetAxis );
    clazz->AddCmd( "fff_GetAxis_i",     'GA__', n_GetAxis );
    clazz->AddCmd( "i_GetAxisRel_i",    'GAR_', n_GetAxisRel );
    clazz->AddCmd( "v_SetAngle_if",     'SANG', n_SetAngle );
    clazz->AddCmd( "f_GetAngle_i",      'GANG', n_GetAngle );
    clazz->AddCmd( "f_GetAngleRate_i",  'GART', n_GetAngleRate );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetMode

    @input
    s(AMotorMode)

    @output
    v

    @info
    AMotorMode can be one of these values:
        - user
        - euler
    For more info lookup dJointSetAMotorMode in the ODE manual.
*/
static
void n_SetMode( void* slf, nCmd* cmd )
{
    nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
    self->SetMode( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetMode

    @input
    v

    @output
    s(AMotorMode)

    @info
    See setmode for a list of valid values for AMotorMode.
*/
static
void n_GetMode( void* slf, nCmd* cmd )
{
    nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
    cmd->Out()->SetS( self->GetModeName() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetNumAxes

    @input
    i(NumAxes)

    @output
    v

    @info
    Set the number of angular axes that will be controlled by the AMotor.
    For more info lookup dJointSetAMotorNumAxes in the ODE manual.
*/
static
void n_SetNumAxes( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  self->SetNumAxes( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetNumAxes

    @input
    v

    @output
    i(NumAxes)

    @info
    Get the number of angular axes that are controlled by the AMotor.
    For more info lookup dJointGetAMotorNumAxes in the ODE manual.
*/
static
void n_GetNumAxes( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  cmd->Out()->SetI( self->GetNumAxes() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetAxis

    @input
    i(AxisNum), i(RelMode), f(x), f(y), f(z)

    @output
    v

    @info
    Set an AMotor axis.
    For more info lookup dJointSetAMotorAxis in the ODE manual.
*/
static
void n_SetAxis( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  int anum = cmd->In()->GetI();
  int rel = cmd->In()->GetI();
  vector3 axis;
  axis.x = cmd->In()->GetF();
  axis.y = cmd->In()->GetF();
  axis.z = cmd->In()->GetF();
  self->SetAxis( anum, rel, axis );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAxis

    @input
    i(AxisNum)

    @output
    f(x), f(y), f(z)

    @info
    Get an AMotor axis.
    For more info lookup dJointGetAMotorAxis in the ODE manual.
*/
static
void n_GetAxis( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  int anum = cmd->In()->GetI();
  vector3 v;
  self->GetAxis( anum, v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAxisRel

    @input
    i(AxisNum)

    @output
    i(RelMode)

    @info
    Get the relative orientation mode for the specified axis.
    For more info lookup dJointGetAMotorAxisRel in the ODE manual.
*/
static
void n_GetAxisRel( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  int anum = cmd->In()->GetI();
  cmd->Out()->SetI( self->GetAxisRel( anum ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetAngle

    @input
    i(AxisNum), f(Angle)

    @output
    v

    @info
    Tell the AMotor what the current angle is along axis AxisNum.
    For more info lookup dJointSetAMotorAngle in the ODE manual.
*/
static
void n_SetAngle( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  int anum = cmd->In()->GetI();
  float angle = cmd->In()->GetF();
  self->SetAngle( anum, angle );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAngle

    @input
    i(AxisNum)

    @output
    f(Angle)

    @info
    Return the current angle for axis AxisNum.
    For more info lookup dJointGetAMotorAngle in the ODE manual.
*/
static
void n_GetAngle( void* slf, nCmd* cmd )
{
  nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
  int anum = cmd->In()->GetI();
  cmd->Out()->SetF( self->GetAngle( anum ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAngleRate

    @input
    i(AxisNum)

    @output
    f(AngleRate)

    @info
    Return the current angle rate for axis AxisNum.
    For more info lookup dJointGetAMotorAngleRate in the ODE manual.
*/
static
void n_GetAngleRate( void* slf, nCmd* cmd )
{
    nOpendeAMotorJoint* self = (nOpendeAMotorJoint*)slf;
    int anum = cmd->In()->GetI();
    cmd->Out()->SetF( self->GetAngleRate( anum ) );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeAMotorJoint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        nCmd* cmd;
            
        // SetNumAxes
        cmd = ps->GetCmd( this, 'SNA_' );
        cmd->In()->SetI( this->GetNumAxes() );
        ps->PutCmd( cmd );
    
        // SetMode
        cmd = ps->GetCmd( this, 'SM__' );
        cmd->In()->SetS( this->GetModeName() );
        ps->PutCmd( cmd );
    
        vector3 v;
        
        for ( int axisNum = 0; axisNum < 3; axisNum++ )
        {
            this->GetAxis( axisNum, v );
            int rel = this->GetAxisRel( axisNum );
            float angle = this->GetAngle( axisNum );
      
            // SetAxis
            cmd = ps->GetCmd( this, 'SA__' );
            cmd->In()->SetI( axisNum );
            cmd->In()->SetI( rel );
            cmd->In()->SetF( v.x );
            cmd->In()->SetF( v.y );
            cmd->In()->SetF( v.z );
            ps->PutCmd( cmd );
      
            // SetAngle
            cmd = ps->GetCmd( this, 'SANG' );
            cmd->In()->SetI( axisNum );
            cmd->In()->SetF( angle );
            ps->PutCmd( cmd );
      
            // save parameters
            for ( int i = 0; i < nOpendeJoint::NUM_JOINT_PARAMS; i++ )
            {
                cmd = ps->GetCmd( this, 'SP__' );
                cmd->In()->SetS( nOpendeJoint::JOINT_PARAM_NAMES[i] );
                cmd->In()->SetI( axisNum );
                cmd->In()->SetF( this->GetParam2( nOpendeJoint::JOINT_PARAM_NAMES[i],
                                                  axisNum ) );
                ps->PutCmd( cmd );
            }
        }

        return true;
    }
    return false;
}
