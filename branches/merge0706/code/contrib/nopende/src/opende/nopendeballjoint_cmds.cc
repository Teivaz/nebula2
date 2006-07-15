//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeballjoint.h"
#include "kernel/npersistserver.h"

static void n_SetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor( void* slf, nCmd* cmd );
static void n_GetAnchor2( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeballjoint
    
    @superclass
    nopendejoint

    @classinfo
    Encapsulates some ball joint related dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetAnchor_fff", 'SA__', n_SetAnchor );
    clazz->AddCmd( "fff_GetAnchor_v", 'GA__', n_GetAnchor );
    clazz->AddCmd( "fff_GetAnchor2_v", 'GA2_', n_GetAnchor2 );
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
    Set the joint anchor point.
    For more info lookup dJointSetBallAnchor in the ODE manual.
*/
static
void n_SetAnchor( void* slf, nCmd* cmd )
{
    nOpendeBallJoint* self = (nOpendeBallJoint*)slf;
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
    Get the joint anchor point.
    For more info lookup dJointGetBallAnchor in the ODE manual.
*/
static
void n_GetAnchor( void* slf, nCmd* cmd )
{
    nOpendeBallJoint* self = (nOpendeBallJoint*)slf;
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
    Get the joint anchor point.
    For more info lookup dJointGetBallAnchor2 in the ODE manual.
*/
static
void n_GetAnchor2( void* slf, nCmd* cmd )
{
    nOpendeBallJoint* self = (nOpendeBallJoint*)slf;
    vector3 v;
    self->GetAnchor2( v );
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
nOpendeBallJoint::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeJoint::SaveCmds( ps ) )
    {
        vector3 v;
        this->GetAnchor( v );
    
        // SetAnchor
        nCmd* cmd = ps->GetCmd( this, 'SA__' );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
