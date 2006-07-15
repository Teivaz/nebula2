//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendeboxgeom.h"
#include "kernel/npersistserver.h"

static void n_SetLengths( void* slf, nCmd* cmd );
static void n_GetLengths( void* slf, nCmd* cmd );
static void n_PointDepth( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeboxgeom
    
    @superclass
    nopendegeom

    @classinfo
    Encapsulates some box geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetLengths_fff",              'SLEN', n_SetLengths );
    clazz->AddCmd( "fff_GetLengths_v",              'GLEN', n_GetLengths );
    clazz->AddCmd( "f_PointDepth_fff",              'PDEP', n_PointDepth );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetLengths

    @input
    f(lx), f(ly), f(lz)

    @output
    v

    @info
*/
static void 
n_SetLengths( void* slf, nCmd* cmd )
{
    nOpendeBoxGeom* self = (nOpendeBoxGeom*)slf;
    float lx = cmd->In()->GetF();
    float ly = cmd->In()->GetF();
    float lz = cmd->In()->GetF();
    self->SetLengths( lx, ly, lz );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetLengths

    @input
    v

    @output
    f(lx), f(ly), f(lz)

    @info
*/
static void 
n_GetLengths( void* slf, nCmd* cmd )
{
    nOpendeBoxGeom* self = (nOpendeBoxGeom*)slf;
    vector3 v;
    self->GetLengths( v );
    cmd->Out()->SetF( v.x );
    cmd->Out()->SetF( v.y );
    cmd->Out()->SetF( v.z );
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
    nOpendeBoxGeom* self = (nOpendeBoxGeom*)slf;
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
bool nOpendeBoxGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        // SetLengths
        nCmd* cmd = ps->GetCmd( this, 'SLEN' );
        vector3 v;
        this->GetLengths( v );
        cmd->In()->SetF( v.x );
        cmd->In()->SetF( v.y );
        cmd->In()->SetF( v.z );
        ps->PutCmd( cmd );
            
        return true;
    }
    return false;
}
