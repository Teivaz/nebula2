//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeuniversaljoint.h"
#include "kernel/npersistserver.h"

static void n_SetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor2( void* slf, nCmd* cmd );
static void n_SetAxis1( void* slf, nCmd* cmd );
static void n_GetAxis1( void* slf, nCmd* cmd );
static void n_SetAxis2( void* slf, nCmd* cmd );
static void n_GetAxis2( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeuniversaljoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some universal joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetAnchor_fff",   'SANC', n_SetAnchor );
    clazz->AddCmd( "fff_GetAnchor_v",   'GANC', n_GetAnchor );
    clazz->AddCmd( "fff_GetAnchor2_v",  'GAN2', n_GetAnchor2 );
    clazz->AddCmd( "v_SetAxis1_fff",    'SAX1', n_SetAxis1 );
    clazz->AddCmd( "fff_GetAxis1_v",    'GAX1', n_GetAxis1 );
    clazz->AddCmd( "v_SetAxis2_fff",    'SAX2', n_SetAxis2 );
    clazz->AddCmd( "fff_GetAxis2_v",    'GAX2', n_GetAxis2 );
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
    Set the anchor point.
    For more info lookup dJointSetUniversalAnchor in the ODE manual.
*/
static
void n_SetAnchor( void* slf, nCmd* cmd )
{
  nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
    Get the anchor point.
    For more info lookup dJointGetUniversalAnchor in the ODE manual.
*/
static
void n_GetAnchor( void* slf, nCmd* cmd )
{
    nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
    Get the anchor point.
    For more info lookup dJointGetUniversalAnchor2 in the ODE manual.
*/
static
void n_GetAnchor2( void* slf, nCmd* cmd )
{
  nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
    Set Axis1.
    For more info lookup dJointSetUniversalAxis1 in the ODE manual.
*/
static
void n_SetAxis1( void* slf, nCmd* cmd )
{
    nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
  Get Axis1.
  For more info lookup dJointGetUniversalAxis1 in the ODE manual.
*/
static
void n_GetAxis1( void* slf, nCmd* cmd )
{
    nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
    Set Axis2.
    For more info lookup dJointSetUniversalAxis2 in the ODE manual.
*/
static
void n_SetAxis2( void* slf, nCmd* cmd )
{
    nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
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
    Get Axis2.
    For more info lookup dJointGetUniversalAxis2 in the ODE manual.
*/
static
void n_GetAxis2( void* slf, nCmd* cmd )
{
    nOpendeUniversalJoint* self = (nOpendeUniversalJoint*)slf;
    vector3 v;
    self->GetAxis2( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeUniversalJoint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        nCmd* cmd;
        vector3 v;
        
        // SetAnchor
        this->GetAnchor( v );
        cmd = ps->GetCmd( this, 'SANC' );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
    
        // SetAxis1
        this->GetAxis1( v );
        cmd = ps->GetCmd( this, 'SAX1' );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
    
        // SetAxis2
        this->GetAxis2( v );
        cmd = ps->GetCmd( this, 'SAX2' );
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
