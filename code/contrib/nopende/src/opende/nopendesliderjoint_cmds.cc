//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendesliderjoint.h"
#include "kernel/npersistserver.h"

static void n_SetAxis( void* slf, nCmd* cmd );
static void n_GetAxis( void* slf, nCmd* cmd );
static void n_GetPosition( void* slf, nCmd* cmd );
static void n_GetPositionRate( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendesliderjoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some slider joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetAxis_fff",         'SAX_', n_SetAxis );
    clazz->AddCmd( "fff_GetAxis_v",         'GAX_', n_GetAxis );
    clazz->AddCmd( "f_GetPosition_v",       'GPOS', n_GetPosition );
    clazz->AddCmd( "f_GetPositionRate_v",   'GPR_', n_GetPositionRate );
    clazz->EndCmds();
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
    Set the slider axis.
    For more info lookup dJointSetSliderAxis in the ODE manual.
*/
static
void n_SetAxis( void* slf, nCmd* cmd )
{
  nOpendeSliderJoint* self = (nOpendeSliderJoint*)slf;
  vector3 v;
  v.x = cmd->In()->GetF();
  v.y = cmd->In()->GetF();
  v.z = cmd->In()->GetF();
  self->SetAxis( v );
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
    Get the slider axis.
    For more info lookup dJointGetSliderAxis in the ODE manual.
*/
static
void n_GetAxis( void* slf, nCmd* cmd )
{
    nOpendeSliderJoint* self = (nOpendeSliderJoint*)slf;
    vector3 v;
    self->GetAxis( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetPosition

    @input
    v

    @output
    f(Position)

    @info
    Get the slider's linear position.
    For more info lookup dJointGetSliderPosition in the ODE manual.
*/
static
void n_GetPosition( void* slf, nCmd* cmd )
{
    nOpendeSliderJoint* self = (nOpendeSliderJoint*)slf;
    cmd->Out()->SetF( self->GetPosition() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetPositionRate

    @input
    v

    @output
    f(PositionRate)

    @info
    For more info lookup dJointGetSliderPositionRate in the ODE manual.
*/
static
void n_GetPositionRate( void* slf, nCmd* cmd )
{
    nOpendeSliderJoint* self = (nOpendeSliderJoint*)slf;
    cmd->Out()->SetF( self->GetPositionRate() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeSliderJoint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        vector3 v;
        this->GetAxis( v );
    
        // SetAxis
        nCmd* cmd = ps->GetCmd( this, 'SAX_' );
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
