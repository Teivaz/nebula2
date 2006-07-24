//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendespace.h"
#include "kernel/npersistserver.h"

static void n_SetCleanup( void* slf, nCmd* cmd );
static void n_GetCleanup( void* slf, nCmd* cmd );
static void n_AddGeom( void* slf, nCmd* cmd );
static void n_RemoveGeom( void* slf, nCmd* cmd );
static void n_ContainsGeom( void* slf, nCmd* cmd );
static void n_GetNumGeoms( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendespace

    @superclass
    nroot

    @classinfo
    Encapsulates some dSpace functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_SetCleanup_b",        'SCLN', n_SetCleanup );
    clazz->AddCmd( "b_GetCleanup_v",        'GCLN', n_GetCleanup );
    clazz->AddCmd( "v_AddGeom_s",           'ADDG', n_AddGeom );
    clazz->AddCmd( "v_RemoveGeom_s",        'REMG', n_RemoveGeom );
    clazz->AddCmd( "b_ContainsGeom_s",      'CONG', n_ContainsGeom );
    clazz->AddCmd( "i_GetNumGeoms_v",       'GNG_', n_GetNumGeoms );
    clazz->EndCmds();
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
    For more info lookup dSpaceSetCleanup in the ODE manual.
*/
static
void n_SetCleanup( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
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
    For more info lookup dSpaceGetCleanup in the ODE manual.
*/
static
void n_GetCleanup( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
  cmd->Out()->SetB( self->GetCleanup() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    AddGeom

    @input
    s(NOH path to an nOpendeGeom instance)

    @output
    v

    @info
    Adds a geom or a space to this space.
*/
static
void n_AddGeom( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
  self->AddGeom( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    RemoveGeom

    @input
    s(NOH path to an nOpendeGeom instance)

    @output
    v

    @info
    Remove the specified geom/space from this space.
*/
static
void n_RemoveGeom( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
  self->RemoveGeom( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    ContainsGeom

    @input
    s(NOH path to an nOpendeGeom instance)

    @output
    b(True/False)

    @info
    Check if the specified geom/space is contained in this space.
*/
static
void n_ContainsGeom( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
  cmd->Out()->SetB( self->ContainsGeom( cmd->In()->GetS() ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetNumGeoms

    @input
    v

    @output
    i(NumGeoms)

    @info
    Return the number of geoms/spaces in this space.
*/
static
void n_GetNumGeoms( void* slf, nCmd* cmd )
{
  nOpendeSpace* self = (nOpendeSpace*)slf;
  cmd->Out()->SetI( self->GetNumGeoms() );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeSpace::SaveCmds( nPersistServer* ps )
{
    if ( nRoot::SaveCmds( ps ) )
    {
        // TODO
        return true;
    }
    return false;
}
