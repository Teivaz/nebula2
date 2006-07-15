//------------------------------------------------------------------------------
//  ncharacter3node_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ncharacter3node.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncharacter3node

    @cppclass
    nCharacter3Node

    @superclass
    nscenenode

    @classinfo
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
nCharacter3Node::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
//        nCmd* cmd;

        return true;
    }
    return false;
}
