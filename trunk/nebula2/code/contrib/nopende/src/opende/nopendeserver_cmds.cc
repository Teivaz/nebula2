//--------------------------------------------------------------------
//  nopendeserver_cmds.cc
//
//  (c) 2004  Vadim Macagon
//
//  nOpendeServer is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "opende/nopendeserver.h"
#include "kernel/npersistserver.h"

static void n_NewTriMesh(void *, nCmd *);
static void n_DeleteTriMesh(void *, nCmd *);
static void n_BeginSurfaces(void *, nCmd *);
static void n_AddSurface(void *, nCmd *);
static void n_EnableSurfParam(void *, nCmd *);
static void n_SetSurfParam(void *, nCmd *);
static void n_EndSurfaces(void *, nCmd *);
static void n_GetSurfaceIndex(void *, nCmd *);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendeserver
    
    @superclass
    nroot

    @classinfo
    Provides facilities for collision detection and physics. 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_NewTriMesh_ssi",      'NTM_', n_NewTriMesh );
    clazz->AddCmd( "v_DeleteTriMesh_s",     'DTM_', n_DeleteTriMesh );
    clazz->AddCmd( "v_BeginSurfaces_i",     'BSUR', n_BeginSurfaces );
    clazz->AddCmd( "v_AddSurface_is",       'ADSF', n_AddSurface );
    clazz->AddCmd( "v_EnableSurfParam_is",  'ESFP', n_EnableSurfParam );
    clazz->AddCmd( "v_SetSurfParam_isf",    'SSFP', n_SetSurfParam );
    clazz->AddCmd( "v_EndSurfaces_v",       'ESUR', n_EndSurfaces );
    clazz->AddCmd( "i_GetSurfaceIndex_s",   'GSFI', n_GetSurfaceIndex );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    NewTriMesh

    @input
    s(MeshId), s(MeshFileName)

    @output
    v

    @info
    Create and load a new collision mesh, MeshId can be used to refer to it
    later (and must be unique within the server), MeshFileName is the path
    to the mesh file on disk (or wherever).
*/
void n_NewTriMesh( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    const char* id = cmd->In()->GetS();
    const char* filename = cmd->In()->GetS();
    int group = cmd->In()->GetI();
    self->NewTriMesh( id, filename, group );
}

//------------------------------------------------------------------------------
/**
    @cmd
    DeleteTriMesh

    @input
    s(MeshId)

    @output
    v

    @info
    Remove a collision mesh from the server, note that the mesh must not be
    in use (otherwise you'll get an assertion). You should only call this
    method to release a mesh when you've destroyed any nOpendeTriMeshGeom
    instances that reference the specified mesh.
*/
void n_DeleteTriMesh( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    const char* id = cmd->In()->GetS();
    self->DeleteTriMesh( id );
}

//------------------------------------------------------------------------------
/**
    @cmd
    BeginSurfaces

    @input
    i(total number of surfaces)

    @output
    v

    @info
    Begin defining surfaces.
*/
void n_BeginSurfaces( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    self->BeginSurfaces( cmd->In()->GetI() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    AddSurface

    @input
    i(SurfaceIndex), s(SurfaceName)

    @output
    v

    @info
    Add a new surface definition.
*/
void n_AddSurface( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    int i = cmd->In()->GetI();
    const char* name = cmd->In()->GetS();
    self->AddSurface( i, name );
}

//------------------------------------------------------------------------------
/**
    @cmd
    EnableSurfParam

    @input
    i(SurfaceIndex), s(ParameterName)

    @output
    v

    @info
    Enable a surface parameter. Here are the available parameters,
    see the ODE user manual for additional info.
    
    fdir1         - corresponds to dContactFDir1
    approxfdir1   - corresponds to dContactApprox1_1
    approxfdir2   - corresponds to dContactApprox1_2
    approxfboth   - corresponds to dContactApprox1
*/
void n_EnableSurfParam( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    int i = cmd->In()->GetI();
    const char* param = cmd->In()->GetS();
    self->EnableSurfaceParam( i, param );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetSurfParam

    @input
    i(SurfaceIndex), s(ParameterName), f(ParameterValue)

    @output
    v

    @info
    Enable and set a surface parameter. Here are the available parameters
    and possible values, see the ODE user manual for additional info.
    
    mu        - Coulomb friction coefficient for friction direction 1
                (corresponds to dSurfaceParameters::mu).
                Value must be in range 0 to infinity, specify -1 to indicate
                infinity.
    mu2       - Coulomb friction coefficient for friction direction 2
                (corresponds to dSurfaceParameters::mu2).
                Value must be in range 0 to infinity, specify -1 to indicate
                infinity.
    cfm       - Corresponds to dSurfaceParameters::soft_cfm.
    erp       - Corresponds to dSurfaceParameters::soft_erp.
    bounce    - Restitution. Value must be in range 0 to 1, 0 means materials
                are not bouncy, 1 means materials are very bouncy
                (corresponds to dSurfaceParameters::bounce).
    bvel      - Minimum velocity necessary for bounce (corresponds to
                dSurfaceParameters::bounce_vel).
    motion1   - Surface velocity in friction direction 1 (corresponds to
                dSurfaceParameters::motion1).
    motion2   - Surface velocity in friction direction 2 (corresponds to
                dSurfaceParameters::motion2).
    slip1     - Coefficient of force dependent slip for friction direction 1
                (corresponds to dSurfaceParameters::slip1)
    slip2     - Coefficient of force dependent slip for friction direction 2
                (corresponds to dSurfaceParameters::slip2)
*/
void n_SetSurfParam( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    int i = cmd->In()->GetI();
    const char* param = cmd->In()->GetS();
    float value = cmd->In()->GetF();
    self->SetSurfaceParam( i, param, value );
}

//------------------------------------------------------------------------------
/**
    @cmd
    EndSurfaces

    @input
    v

    @output
    v

    @info
    Finish defining surfaces.
*/
void n_EndSurfaces( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    self->EndSurfaces();
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetSurfaceIndex

    @input
    s(SurfaceName)

    @output
    i(SurfaceIndex)

    @info
    Obtain the index of the specified surface.
*/
void n_GetSurfaceIndex( void* slf, nCmd* cmd )
{
    nOpendeServer* self = (nOpendeServer*)slf;
    cmd->Out()->SetI( self->GetSurfaceIndex( cmd->In()->GetS() ) );
}

