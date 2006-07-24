//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendehinge2joint.h"
#include "kernel/npersistserver.h"

static void n_SetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor2( void* slf, nCmd* cmd );
static void n_SetAxis1( void* slf, nCmd* cmd );
static void n_GetAxis1( void* slf, nCmd* cmd );
static void n_SetAxis2( void* slf, nCmd* cmd );
static void n_GetAxis2( void* slf, nCmd* cmd );
static void n_GetAngle1( void* slf, nCmd* cmd );
static void n_GetAngle1Rate( void* slf, nCmd* cmd );
static void n_GetAngle2Rate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendehinge2joint

    @superclass
    nopendejoint

    @classinfo
    Encapsulates some hinge2 joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetAnchor_fff",       'SANC', n_SetAnchor );
    clazz->AddCmd( "fff_GetAnchor_v",       'GANC', n_GetAnchor );
    clazz->AddCmd( "fff_GetAnchor2_v",      'GAN2', n_GetAnchor2 );
    clazz->AddCmd( "v_SetAxis1_fff",        'SAX1', n_SetAxis1 );
    clazz->AddCmd( "fff_GetAxis1_v",        'GAX1', n_GetAxis1 );
    clazz->AddCmd( "v_SetAxis2_fff",        'SAX2', n_SetAxis2 );
    clazz->AddCmd( "fff_GetAxis2_v",        'GAX2', n_GetAxis2 );
    clazz->AddCmd( "f_GetAngle1_v",         'GA1_', n_GetAngle1 );
    clazz->AddCmd( "f_GetAngle1Rate_v",     'GA1R', n_GetAngle1Rate );
    clazz->AddCmd( "f_GetAngle2Rate_v",     'GA2R', n_GetAngle2Rate );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetAnchor

    @input
    f(x), f(y), f(z)

    @output
    v

    @info
    Set hinge-2 anchor.
    For more info lookup dJointSetHinge2Anchor in the ODE manual.
*/
static
void n_SetAnchor( void* slf, nCmd* cmd )
{
  nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->SetAnchor( v );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAnchor

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
    Get hinge-2 anchor.
    For more info lookup dJointGetHinge2Anchor in the ODE manual.
*/
static
void n_GetAnchor( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    vector3 v;
    self->GetAnchor( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAnchor2

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
    Get hinge-2 anchor.
    For more info lookup dJointGetHinge2Anchor2 in the ODE manual.
*/
static
void n_GetAnchor2( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    vector3 v;
    self->GetAnchor2( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetAxis1

    @input
    f(x), f(y), f(z)

    @output
    v

    @info
    For more info lookup dJointSetHinge2Axis1 in the ODE manual.
*/
static
void n_SetAxis1( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    self->SetAxis1( v );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAxis1

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
    For more info lookup dJointGetHinge2Axis1 in the ODE manual.
*/
static
void n_GetAxis1( void* slf, nCmd* cmd )
{
  nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
  vector3 v;
  self->GetAxis1( v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetAxis2

    @input
    f(x), f(y), f(z)

    @output
    v

    @info
    For more info lookup dJointSetHinge2Axis2 in the ODE manual.
*/
static
void n_SetAxis2( void* slf, nCmd* cmd )
{
  nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->SetAxis2( v );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAxis2

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
    For more info lookup dJointGetHinge2Axis2 in the ODE manual.
*/
static
void n_GetAxis2( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    vector3 v;
    self->GetAxis2( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAngle1

    @input
    v

    @output
    f(Angle)

    @info
    For more info lookup dJointGetHinge2Angle1 in the ODE manual.
*/
static
void n_GetAngle1( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    cmd->Out()->SetF( self->GetAngle1() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAngle1Rate

    @input
    v

    @output
    f(AngleRate)

    @info
    For more info lookup dJointGetHinge2Angle1Rate in the ODE manual.
*/
static
void n_GetAngle1Rate( void* slf, nCmd* cmd )
{
  nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
  cmd->Out()->SetF( self->GetAngle1Rate() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetAngle2Rate

    @input
    v

    @output
    f(AngleRate)

    @info
    For more info lookup dJointGetHinge2Angle2Rate in the ODE manual.
*/
static
void n_GetAngle2Rate( void* slf, nCmd* cmd )
{
    nOpendeHinge2Joint* self = (nOpendeHinge2Joint*)slf;
    cmd->Out()->SetF( self->GetAngle2Rate() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeHinge2Joint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        nCmd* cmd;
        vector3 v;

        // SetAnchor
        cmd = ps->GetCmd( this, 'SANC' );
        this->GetAnchor( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );

        // SetAxis1
        cmd = ps->GetCmd( this, 'SAX1' );
        this->GetAxis1( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );

        // SetAxis2
        cmd = ps->GetCmd( this, 'SAX2' );
        this->GetAxis1( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );

        // save parameters
        for ( int axisNum = 0; axisNum < 3; axisNum++ )
        {
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
