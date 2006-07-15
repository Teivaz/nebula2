//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendetrimeshgeom.h"
#include "opende/nopendetrimesh.h"
#include "kernel/npersistserver.h"

static void n_SetMesh( void* slf, nCmd* cmd );
static void n_GetMesh( void* slf, nCmd* cmd );
static void n_EnableTC( void* slf, nCmd* cmd );
static void n_IsTCEnabled( void* slf, nCmd* cmd );
static void n_ClearTCCache( void* slf, nCmd* cmd );
static void n_GetTriangle( void* slf, nCmd* cmd );
static void n_GetPoint( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendetrimeshgeom
    
    @superclass
    nopendegeom

    @classinfo
    Encapsulates some tri mesh geom functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetMesh_s",               'SMSH', n_SetMesh );
    clazz->AddCmd( "s_GetMesh_v",               'GMSH', n_GetMesh );
    clazz->AddCmd( "v_EnableTC_sb",             'ETC_', n_EnableTC );
    clazz->AddCmd( "b_IsTCEnabled_s",           'ITCE', n_IsTCEnabled );
    clazz->AddCmd( "v_ClearTCCache_v",          'CTCC', n_ClearTCCache );
    clazz->AddCmd( "fffffffff_GetTriangle_i",   'GTRI', n_GetTriangle );
    clazz->AddCmd( "fff_GetPoint_iff",          'GPNT', n_GetPoint );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetMesh

    @input
    s(TriMeshId)

    @output
    v

    @info
*/
static void 
n_SetMesh( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    self->SetMesh( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetMesh

    @input
    v

    @output
    s(TriMeshId)

    @info
*/
static void 
n_GetMesh( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    cmd->Out()->SetS( self->GetMesh() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    EnableTC

    @input
    s(GeomClassName), b(True/False)

    @output
    v

    @info
*/
static void 
n_EnableTC( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    const char* name = cmd->In()->GetS();
    bool enable = cmd->In()->GetB();
    self->EnableTC( name, enable );
}

//------------------------------------------------------------------------------
/**
    @cmd
    IsTCEnabled

    @input
    s(GeomClassName)

    @output
    b(True/False)

    @info
*/
static void 
n_IsTCEnabled( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    const char* name = cmd->In()->GetS();
    cmd->Out()->SetB( self->IsTCEnabled( name ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    ClearTCCache

    @input
    v

    @output
    v

    @info
*/
static void 
n_ClearTCCache( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    self->ClearTCCache();
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetTriangle

    @input
    i(Index)

    @output
    f(v0.x), f(v0.y), f(v0.z),
    f(v1.x), f(v1.y), f(v1.z),
    f(v2.x), f(v2.y), f(v2.z)

    @info
*/
static void 
n_GetTriangle( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    int i = cmd->In()->GetI();
    vector3 v0, v1, v2;
    self->GetTriangle( i, v0, v1, v2 );
    cmd->Out()->SetF( v0.x );
    cmd->Out()->SetF( v0.y );
    cmd->Out()->SetF( v0.z );
    cmd->Out()->SetF( v1.x );
    cmd->Out()->SetF( v1.y );
    cmd->Out()->SetF( v1.z );
    cmd->Out()->SetF( v2.x );
    cmd->Out()->SetF( v2.y );
    cmd->Out()->SetF( v2.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetPoint

    @input
    i(Index), f(u), f(v)

    @output
    f(x), f(y), f(z)

    @info
*/
static void 
n_GetPoint( void* slf, nCmd* cmd )
{
    nOpendeTriMeshGeom* self = (nOpendeTriMeshGeom*)slf;
    int i = cmd->In()->GetI();
    float u = cmd->In()->GetF();
    float v = cmd->In()->GetF();
    vector3 out;
    self->GetPoint( i, u, v, out );
    cmd->Out()->SetF( out.x );
    cmd->Out()->SetF( out.y );
    cmd->Out()->SetF( out.z );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeTriMeshGeom::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeGeom::SaveCmds( ps ) )
    {
        nCmd* cmd;
        
        // SetMesh
        if ( this->triMesh )
        {
            cmd = ps->GetCmd( this, 'SMSH' );
            cmd->In()->SetS( this->triMesh->GetName() );
            ps->PutCmd( cmd );
        }
        
        // EnableTC sphere
        cmd = ps->GetCmd( this, 'ETC_' );
        cmd->In()->SetS( "sphere" );
        if ( this->IsTCEnabled( dSphereClass ) )
            cmd->In()->SetB( true );
        else
            cmd->In()->SetB( false );
        ps->PutCmd( cmd );
        
        // EnableTC box
        cmd = ps->GetCmd( this, 'ETC_' );
        cmd->In()->SetS( "box" );
        if ( this->IsTCEnabled( dBoxClass ) )
            cmd->In()->SetB( true );
        else
            cmd->In()->SetB( false );
        ps->PutCmd( cmd );
            
        // EnableTC capsule
        cmd = ps->GetCmd( this, 'ETC_' );
        cmd->In()->SetS( "capsule" );
        if ( this->IsTCEnabled( dCCylinderClass ) )
            cmd->In()->SetB( true );
        else
            cmd->In()->SetB( false );
        ps->PutCmd( cmd );
        
        // EnableTC cylinder
        cmd = ps->GetCmd( this, 'ETC_' );
        cmd->In()->SetS( "cylinder" );
        if ( this->IsTCEnabled( dCylinderClass ) )
            cmd->In()->SetB( true );
        else
            cmd->In()->SetB( false );
        ps->PutCmd( cmd );
        
        return true;
    }
    return false;
}
