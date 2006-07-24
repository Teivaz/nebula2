//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeplanegeom.h"
#include "kernel/npersistserver.h"

static void n_SetParams( void* slf, nCmd* cmd );
static void n_GetParams( void* slf, nCmd* cmd );
static void n_PointDepth( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeplanegeom

    @superclass
    nopendegeom

    @classinfo
    Encapsulates some plane geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetParams_ffff",              'SPRM', n_SetParams );
    clazz->AddCmd( "ffff_GetParams_v",              'GPRM', n_GetParams );
    clazz->AddCmd( "f_PointDepth_fff",              'PDEP', n_PointDepth );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetParams

    @input
    f(a), f(b), f(c), f(d)

    @output
    v

    @info
*/
static void
n_SetParams( void* slf, nCmd* cmd )
{
    nOpendePlaneGeom* self = (nOpendePlaneGeom*)slf;
    float a = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float c = cmd->In()->GetF();
    float d = cmd->In()->GetF();
    self->SetParams( a, b, c, d );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetParams

    @input
    v

    @output
    f(a), f(b), f(c), f(d)

    @info
*/
static void
n_GetParams( void* slf, nCmd* cmd )
{
    nOpendePlaneGeom* self = (nOpendePlaneGeom*)slf;
    vector4 v;
    self->GetParams( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
    cmd->Out()->SetF( v.w );
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
    nOpendePlaneGeom* self = (nOpendePlaneGeom*)slf;
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
bool nOpendePlaneGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        // SetParams
        nCmd* cmd = ps->GetCmd( this, 'SPRM' );
        vector4 v;
        this->GetParams( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        cmd->In()->SetF( v.w );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
