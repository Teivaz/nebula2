//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendehashspace.h"
#include "kernel/npersistserver.h"

static void n_GetLevels( void* slf, nCmd* cmd );
static void n_SetLevels( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendehashspace
    
    @superclass
    nopendespace

    @classinfo
    Encapsulates some dSpace functions.
    You must call Create() to initialize the space before using it.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "ii_GetLevels_v",    'SLVL', n_GetLevels );
    clazz->AddCmd( "v_SetLevels_ii",    'GLVL', n_SetLevels );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetLevels

    @input
    v

    @output
    i(minLevel), i(maxLevel)

    @info
*/
static
void n_GetLevels( void* slf, nCmd* cmd )
{
  nOpendeHashSpace* self = (nOpendeHashSpace*)slf;
  int minLevel, maxLevel;
  self->GetLevels( &minLevel, &maxLevel );
  cmd->Out()->SetI( minLevel );
  cmd->Out()->SetI( maxLevel );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetLevels

    @input
    i(minLevel), i(maxLevel)

    @output
    v

    @info
*/
static
void n_SetLevels( void* slf, nCmd* cmd )
{
  nOpendeHashSpace* self = (nOpendeHashSpace*)slf;
  int minLevel = cmd->In()->GetI();
  int maxLevel = cmd->In()->GetI();
  self->SetLevels( minLevel, maxLevel );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeHashSpace::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeSpace::SaveCmds( ps ) )
    {
        // TODO
        return true;
    }
    return false;
}
