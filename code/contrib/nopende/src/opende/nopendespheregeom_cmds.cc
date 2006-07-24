//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendespheregeom.h"
#include "kernel/npersistserver.h"

static void n_SetRadius( void* slf, nCmd* cmd );
static void n_GetRadius( void* slf, nCmd* cmd );
static void n_PointDepth( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendespheregeom

    @superclass
    nopendegeom

    @classinfo
    Encapsulates some sphere geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetRadius_f",                 'SRAD', n_SetRadius );
    clazz->AddCmd( "f_GetRadius_v",                 'GRAD', n_GetRadius );
    clazz->AddCmd( "f_PointDepth_fff",              'PDEP', n_PointDepth );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetRadius

    @input
    f(Radius)

    @output
    v

    @info
*/
static void
n_SetRadius( void* slf, nCmd* cmd )
{
    nOpendeSphereGeom* self = (nOpendeSphereGeom*)slf;
    self->SetRadius( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetRadius

    @input
    v

    @output
    f(Radius)

    @info
*/
static void
n_GetRadius( void* slf, nCmd* cmd )
{
    nOpendeSphereGeom* self = (nOpendeSphereGeom*)slf;
    cmd->Out()->SetF( self->GetRadius() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    PointDepth

    @input
    f(x), f(y), f(z)

    @output
    f(depth)

    @info
*/
static void
n_PointDepth( void* slf, nCmd* cmd )
{
    nOpendeSphereGeom* self = (nOpendeSphereGeom*)slf;
    vector3 v;
    v.x = cmd->In()->GetF();
    v.y = cmd->In()->GetF();
    v.z = cmd->In()->GetF();
    cmd->Out()->SetF( self->PointDepth( v ) );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeSphereGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        // SetRadius
        nCmd* cmd = ps->GetCmd( this, 'SRAD' );
        cmd->In()->SetF( this->GetRadius() );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
