//------------------------------------------------------------------------------
//  ncharacter3skinanimator_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ncharacter3skinanimator.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nCharacter3SkinAnimator

    @cppclass
    nCharacter3SkinAnimator

    @superclass
    nanimator

    @classinfo
    Provide an animated joint skeleton for nSkinShape node.
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
/*
bool
nCharacter3SkinAnimator::SaveCmds(nPersistServer* ps)
{
    if (nSkinAnimator::SaveCmds(ps))
    {
        return true;
    }
    return false;
}
*/
