//------------------------------------------------------------------------------
//  nparticleserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleserver2.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nparticleserver

    @cppclass
    nParticleServer

    @superclass
    nparticleserver

    @classinfo
    Server class of the particle subsystem.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->EndCmds();
}
