//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeworld.h"
#include "kernel/npersistserver.h"

static void n_setgravity( void* slf, nCmd* cmd );
static void n_getgravity( void* slf, nCmd* cmd );
static void n_seterp( void* slf, nCmd* cmd );
static void n_geterp( void* slf, nCmd* cmd );
static void n_setcfm( void* slf, nCmd* cmd );
static void n_getcfm( void* slf, nCmd* cmd );
static void n_step( void* slf, nCmd* cmd );
static void n_impulsetoforce( void* slf, nCmd* cmd );
static void n_stepfast1( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeworld
    
    @superclass
    nroot

    @classinfo
    Encapsulates dWorld functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetGravity_fff",          'SGRV', n_setgravity );
    clazz->AddCmd( "fff_GetGravity_v",          'GGRV', n_getgravity );
    clazz->AddCmd( "v_SetERP_f",                'SERP', n_seterp );
    clazz->AddCmd( "f_GetERP_v",                'GERP', n_geterp );
    clazz->AddCmd( "v_SetCFM_f",                'SCFM', n_setcfm );
    clazz->AddCmd( "f_GetCFM_v",                'GCFM', n_getcfm );
    clazz->AddCmd( "v_Step_f",                  'STEP', n_step );
    clazz->AddCmd( "fff_ImpulseToForce_ffff",   'ITOF', n_impulsetoforce );
    clazz->AddCmd( "v_StepFast1_fi",            'STPF', n_stepfast1 );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetGravity

    @input
    f(x), f(y), f(z)

    @output
    v

    @info
    Set world gravity.
*/
static 
void n_setgravity( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    vector3 in;
    in.x = cmd->In()->GetF();
    in.y = cmd->In()->GetF();
    in.z = cmd->In()->GetF();
    self->SetGravity( in );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetGravity

    @input
    v

    @output
    f(x), f(y), f(z)

    @info
    Get world gravity.
*/
static 
void n_getgravity( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    vector3 out;
    self->GetGravity( out );
    cmd->Out()->SetF( out.x );
    cmd->Out()->SetF( out.y );
    cmd->Out()->SetF( out.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetERP

    @input
    f(ERP)

    @output
    v

    @info
    Set global ERP value.
*/
static 
void n_seterp( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    self->SetERP( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetERP

    @input
    v

    @output
    f(ERP)

    @info
    Get global ERP value.
*/
static 
void n_geterp( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    cmd->Out()->SetF( self->GetERP() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetCFM

    @input
    f(CFM)

    @output
    v

    @info
    Set global CFM value.
*/
static 
void n_setcfm( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    self->SetCFM( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetCFM

    @input
    v

    @output
    f(CFM)

    @info
    Get global CFM value.
*/
static 
void n_getcfm( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    cmd->Out()->SetF( self->GetCFM() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    Step

    @input
    f(StepSize)

    @output
    v

    @info
    Step the world.
*/
static 
void n_step( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    self->Step( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    ImpulseToForce

    @input
    f(StepSize), f(ImpulseX), f(ImpulseY), f(ImpulseZ)

    @output
    f(ForceX), f(ForceY), f(ForceZ)

    @info
    Convert impulse to force.
*/
static 
void n_impulsetoforce( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    vector3 in, out;
    float stepSize = cmd->In()->GetF();
    in.x = cmd->In()->GetF();
    in.y = cmd->In()->GetF();
    in.z = cmd->In()->GetF();
    self->ImpulseToForce( stepSize, in, out );
    cmd->Out()->SetF( out.x );
    cmd->Out()->SetF( out.y );
    cmd->Out()->SetF( out.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    StepFast1

    @input
    f(StepSize), i(MaxIterations)

    @output
    v

    @info
    Step the world using StepFast.
*/
static 
void n_stepfast1( void* slf, nCmd* cmd )
{
    nOpendeWorld* self = (nOpendeWorld*)slf;
    float stepSize = cmd->In()->GetF();
    int maxIterations = cmd->In()->GetI();
    self->StepFast1( stepSize, maxIterations );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeWorld::SaveCmds( nPersistServer* ps )
{
    if ( nRoot::SaveCmds( ps ) )
    {
        nCmd* cmd;

        // SetGravity
        cmd = ps->GetCmd( this, 'SGRV' );
        vector3 v;
        this->GetGravity( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
        
        // SetERP
        cmd = ps->GetCmd( this, 'SERP' );
        cmd->In()->SetF( this->GetERP() );
        ps->PutCmd( cmd );
        
        // SetCFM
        cmd = ps->GetCmd( this, 'SCFM' );
        cmd->In()->SetF( this->GetCFM() );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
