//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendetransformgeom.h"
#include "kernel/npersistserver.h"

static void n_SetGeom( void* slf, nCmd* cmd );
static void n_GetGeom( void* slf, nCmd* cmd );
static void n_SetCleanup( void* slf, nCmd* cmd );
static void n_GetCleanup( void* slf, nCmd* cmd );
static void n_SetInfo( void* slf, nCmd* cmd );
static void n_GetInfo( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendetransformgeom

    @superclass
    nopendegeom

    @classinfo
    Encapsulates some transform geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetGeom_s",           'SGEO', n_SetGeom );
    clazz->AddCmd( "s_GetGeom_v",           'GGEO', n_GetGeom );
    clazz->AddCmd( "v_SetCleanup_b",        'SCLN', n_SetCleanup );
    clazz->AddCmd( "b_GetCleanup_v",        'GCLN', n_GetCleanup );
    clazz->AddCmd( "v_SetInfo_i",           'SINF', n_SetInfo );
    clazz->AddCmd( "i_GetInfo_v",           'GINF', n_GetInfo );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetGeom

    @input
    s(NOH path to an nOpendeGeom instance)

    @output
    v

    @info
*/
static void
n_SetGeom( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    self->SetGeom( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetGeom

    @input
    v

    @output
    s(NOH path to an nOpendeGeom instance)

    @info
*/
static void
n_GetGeom( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    cmd->Out()->SetS( self->GetGeomName() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetCleanup

    @input
    b(True/False)

    @output
    v

    @info
*/
static void
n_SetCleanup( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    self->SetCleanup( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetCleanup

    @input
    v

    @output
    b(True/False)

    @info
*/
static void
n_GetCleanup( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    cmd->Out()->SetB( self->GetCleanup() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetInfo

    @input
    b(True/False)

    @output
    v

    @info
*/
static void
n_SetInfo( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    self->SetInfo( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetInfo

    @input
    v

    @output
    b(True/False)

    @info
*/
static void
n_GetInfo( void* slf, nCmd* cmd )
{
    nOpendeTransformGeom* self = (nOpendeTransformGeom*)slf;
    cmd->Out()->SetI( self->GetInfo() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeTransformGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        nCmd* cmd;

        // SetGeom
        cmd = ps->GetCmd( this, 'SGEO' );
        cmd->In()->SetS( this->geomName.Get() );
        ps->PutCmd( cmd );

        // SetCleanup
        cmd = ps->GetCmd( this, 'SCLN' );
        cmd->In()->SetB( this->GetCleanup() );
        ps->PutCmd( cmd );

        // SetInfo
        cmd = ps->GetCmd( this, 'SINF' );
        cmd->In()->SetI( this->GetInfo() );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
