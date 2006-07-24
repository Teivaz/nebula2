//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendequadtreespace.h"
#include "kernel/npersistserver.h"

static void n_Create2( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendequadtreespace

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
    clazz->AddCmd( "v_Create2_sffffffi", 'CRE2', n_Create2 );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    Create2

    @input
    s(nOpendeSpaceName), f(Center.x), f(Center.y), f(Center.z),
    f(Extents.x), f(Extents.y), f(Extents.z), i(depth)

    @output
    v

    @info
    nOpendeSpaceName should be the NOH path of the parent space, use "none"
    to indicate this space doesn't have a parent.
*/
static
void n_Create2( void* slf, nCmd* cmd )
{
  nOpendeQuadTreeSpace* self = (nOpendeQuadTreeSpace*)slf;
  const char* parent = cmd->In()->GetS();
  vector3 center, extents;
  center.x = cmd->In()->GetF();
  center.y = cmd->In()->GetF();
  center.z = cmd->In()->GetF();
  extents.x = cmd->In()->GetF();
  extents.y = cmd->In()->GetF();
  extents.z = cmd->In()->GetF();
  int depth = cmd->In()->GetI();
  self->Create( parent, center, extents, depth );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
nOpendeQuadTreeSpace::SaveCmds( nPersistServer* ps )
{
    if ( nOpendeSpace::SaveCmds( ps ) )
    {
        // TODO
        return true;
    }
    return false;
}
