//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopenderaygeom.h"
#include "kernel/npersistserver.h"

static void n_SetLength( void* slf, nCmd* cmd );
static void n_GetLength( void* slf, nCmd* cmd );
static void n_Set( void* slf, nCmd* cmd );
static void n_Get( void* slf, nCmd* cmd );
static void n_SetParams( void* slf, nCmd* cmd );
static void n_GetParams( void* slf, nCmd* cmd );
static void n_SetClosestHit( void* slf, nCmd* cmd );
static void n_GetClosestHit( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopenderaygeom

    @superclass
    nopendegeom

    @classinfo
    Encapsulates some ray geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetLength_f",                 'SLEN', n_SetLength );
    clazz->AddCmd( "f_GetLength_v",                 'GLEN', n_GetLength );
    clazz->AddCmd( "v_Set_ffffff",                  'SET_', n_Set );
    clazz->AddCmd( "ffffff_Get_v",                  'GET_', n_Get );
    clazz->AddCmd( "v_SetParams_bb",                'SPRM', n_SetParams );
    clazz->AddCmd( "bb_GetParams_v",                'GPRM', n_GetParams );
    clazz->AddCmd( "v_SetClosestHit_b",             'SCHT', n_SetClosestHit );
    clazz->AddCmd( "b_GetClosestHit_v",             'GCHT', n_GetClosestHit );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetLength

    @input
    f(length)

    @output
    v

    @info
*/
static void
n_SetLength( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    self->SetLength( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetLength

    @input
    v

    @output
    f(length)

    @info
*/
static void
n_GetLength( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    cmd->Out()->SetF( self->GetLength() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    Set

    @input
    f(px), f(py), f(pz),
    f(dx), f(dy), f(dz)

    @output
    v

    @info
*/
static void
n_Set( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    vector3 p, d;
    p.x = cmd->In()->GetF();
    p.y = cmd->In()->GetF();
    p.z = cmd->In()->GetF();
    d.x = cmd->In()->GetF();
    d.y = cmd->In()->GetF();
    d.z = cmd->In()->GetF();
    self->Set( p, d );
}

//------------------------------------------------------------------------------
/**
    @cmd
    Get

    @input
    v

    @output
    f(px), f(py), f(pz),
    f(dx), f(dy), f(dz)

    @info
*/
static void
n_Get( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    vector3 p, d;
    self->Get( p, d );
    cmd->Out()->SetF( p.x );
    cmd->Out()->SetF( p.y );
    cmd->Out()->SetF( p.z );
    cmd->Out()->SetF( d.x );
    cmd->Out()->SetF( d.y );
    cmd->Out()->SetF( d.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetParams

    @input
    b(firstContact), b(backfaceCull)

    @output
    v

    @info
*/
static void
n_SetParams( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    bool firstContact = cmd->In()->GetB();
    bool backfaceCull = cmd->In()->GetB();
    self->SetParams( firstContact, backfaceCull );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetParams

    @input
    v

    @output
    b(firstContact), b(backfaceCull)

    @info
*/
static void
n_GetParams( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    bool firstContact, backfaceCull;
    self->GetParams( &firstContact, &backfaceCull );
    cmd->Out()->SetB( firstContact );
    cmd->Out()->SetB( backfaceCull );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetClosestHit

    @input
    b(closestHit)

    @output
    v

    @info
*/
static void
n_SetClosestHit( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    self->SetClosestHit( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetClosestHit

    @input
    v

    @output
    b(closestHit)

    @info
*/
static void
n_GetClosestHit( void* slf, nCmd* cmd )
{
    nOpendeRayGeom* self = (nOpendeRayGeom*)slf;
    cmd->Out()->SetB( self->GetClosestHit() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeRayGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        nCmd* cmd;

        // SetLength
        cmd = ps->GetCmd( this, 'SLEN' );
        cmd->In()->SetF( this->GetLength() );
        ps->PutCmd( cmd );

        // Set
        cmd = ps->GetCmd( this, 'SET_' );
        vector3 p, d;
        this->Get( p, d );
        cmd->In()->SetF( p.x );
        cmd->In()->SetF( p.y );
        cmd->In()->SetF( p.z );
        cmd->In()->SetF( d.x );
        cmd->In()->SetF( d.y );
        cmd->In()->SetF( d.z );
        ps->PutCmd( cmd );

        // SetParams
        cmd = ps->GetCmd( this, 'SPRM' );
        bool a, b;
        this->GetParams( &a, &b );
        cmd->In()->SetB( a );
        cmd->In()->SetB( b );
        ps->PutCmd( cmd );

        // SetClosestHit
        cmd = ps->GetCmd( this, 'SCHT' );
        cmd->In()->SetB( this->GetClosestHit() );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
