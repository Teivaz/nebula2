//------------------------------------------------------------------------------
//  nterraingrassnode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ncterrain2/nterraingrassnode.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nterraingrassnode

    @superclass
    nmaterialnode

    @classinfo
    A specialized terrain grass renderer. Gets list of grass triangles
    from application.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainGrassNode::SaveCmds(nPersistServer* ps)
{
    if (nMaterialNode::SaveCmds(ps))
    {
        return true;
    }
    return false;
}
