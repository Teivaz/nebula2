//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendehingejoint.h"
#include "kernel/npersistserver.h"

static void n_SetAnchor( void* slf, nCmd* cmd );
static void n_SetAxis( void* slf, nCmd* cmd );
static void n_GetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor2( void* slf, nCmd* cmd );
static void n_GetAxis( void* slf, nCmd* cmd );
static void n_GetAngle( void* slf, nCmd* cmd );
static void n_GetAngleRate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendehingejoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some hinge joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetAnchor_fff",   'SANC', n_SetAnchor );
    clazz->AddCmd( "v_SetAxis_fff",     'SAX_', n_SetAxis );
    clazz->AddCmd( "fff_GetAnchor_v",   'GANC', n_GetAnchor );
    clazz->AddCmd( "fff_GetAnchor2_v",  'GAN2', n_GetAnchor2 );
    clazz->AddCmd( "fff_GetAxis_v",     'GAX_', n_GetAxis );
    clazz->AddCmd( "f_GetAngle_v",      'GANG', n_GetAngle );
    clazz->AddCmd( "f_GetAngleRate_v",  'GAR_', n_GetAngleRate );
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
  Set hinge anchor.
  For more info lookup dJointSetHingeAnchor in the ODE manual.
*/
static
void
n_SetAnchor( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->SetAnchor( v );
}

//------------------------------------------------------------------------------
/**
  @cmd
  SetAxis

  @input
  f(x), f(y), f(z)

  @output
  v

  @info
  Set the hinge axis.
  For more info lookup dJointSetHingeAxis in the ODE manual.
*/
static
void
n_SetAxis( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->SetAxis( v );
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
  Get the hinge anchor point.
  For more info lookup dJointGetHingeAnchor in the ODE manual.
*/
static
void n_GetAnchor( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
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
  Get the hinge anchor point.
  For more info lookup dJointGetHingeAnchor2 in the ODE manual.
*/
static
void n_GetAnchor2( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  vector3 v;
  self->GetAnchor2( v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetAxis

  @input
  v

  @output
  f(x), f(y), f(z)

  @info
  Get the hinge axis.
  For more info lookup dJointGetHingeAxis in the ODE manual.
*/
static
void n_GetAxis( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  vector3 v;
  self->GetAxis( v );
  cmd->Out()->SetF( v.x );
  cmd->Out()->SetF( v.y );
  cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetAngle

  @input
  v

  @output
  f(Angle)

  @info
  Get the hinge angle.
  For more info lookup dJointGetHingeAngle in the ODE manual.
*/
static
void n_GetAngle( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  cmd->Out()->SetF( self->GetAngle() );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetAngleRate

  @input
  v

  @output
  f(AngleRate)

  @info
  For more info lookup dJointGetHingeAngleRate in the ODE manual.
*/
static
void n_GetAngleRate( void* slf, nCmd* cmd )
{
  nOpendeHingeJoint* self = (nOpendeHingeJoint*)slf;
  cmd->Out()->SetF( self->GetAngleRate() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeHingeJoint::SaveCmds( nPersistServer* ps )
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
    
        // SetAxis
        cmd = ps->GetCmd( this, 'SAX_' );
        this->GetAxis( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
    
        // save parameters
        for ( int i = 0; i < nOpendeJoint::NUM_JOINT_PARAMS; i++ )
        {
            cmd = ps->GetCmd( this, 'SP__' );
            cmd->In()->SetS( nOpendeJoint::JOINT_PARAM_NAMES[i] );
            cmd->In()->SetI( 0 );
            cmd->In()->SetF( this->GetParam2( nOpendeJoint::JOINT_PARAM_NAMES[i], 0 ) );
            ps->PutCmd( cmd );
        }

        return true;
    }
    return false;
}
