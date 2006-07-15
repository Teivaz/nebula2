//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendecapsulegeom.h"
#include "kernel/npersistserver.h"

static void n_SetParams( void* slf, nCmd* cmd );
static void n_GetParams( void* slf, nCmd* cmd );
static void n_PointDepth( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendecapsulegeom
    
    @superclass
    nopendegeom

    @classinfo
    Encapsulates some capsule/ccylinder geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetParams_ff",                'SPRM', n_SetParams );
    clazz->AddCmd( "ff_GetParams_v",                'GPRM', n_GetParams );
    clazz->AddCmd( "f_PointDepth_fff",              'PDEP', n_PointDepth );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetParams

    @input
    f(radius), f(length)

    @output
    v

    @info
*/
static void 
n_SetParams( void* slf, nCmd* cmd )
{
    nOpendeCapsuleGeom* self = (nOpendeCapsuleGeom*)slf;
    float radius = cmd->In()->GetF();
    float length = cmd->In()->GetF();
    self->SetParams( radius, length );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetParams

    @input
    v

    @output
    f(radius), f(length)

    @info
*/
static void 
n_GetParams( void* slf, nCmd* cmd )
{
    nOpendeCapsuleGeom* self = (nOpendeCapsuleGeom*)slf;
    float radius, length;
    self->GetParams( &radius, &length );
    cmd->Out()->SetF( radius );
    cmd->Out()->SetF( length );
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
    nOpendeCapsuleGeom* self = (nOpendeCapsuleGeom*)slf;
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
bool nOpendeCapsuleGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        // SetParams
        nCmd* cmd = ps->GetCmd( this, 'SPRM' );
        float radius, length;
        this->GetParams( &radius, &length );
        cmd->In()->SetF( radius );
        cmd->In()->SetF( length );
        ps->PutCmd( cmd );
            
        return true;
    }
    return false;
}
