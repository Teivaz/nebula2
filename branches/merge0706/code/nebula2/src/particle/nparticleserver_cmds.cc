//------------------------------------------------------------------------------
//  nparticleserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleserver.h"

static void n_setenabled(void* slf, nCmd* cmd);
static void n_isenabled(void* slf, nCmd* cmd);

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
    cl->AddCmd("v_setenabled_b", 'SENB', n_setenabled);
    cl->AddCmd("b_isenabled_v", 'GENB', n_isenabled);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setenabled
    @input
    b(Enabled)
    @output
    v
    @info
    Enable/disable the particle subsystem
*/
static void
n_setenabled(void* slf, nCmd* cmd)
{
    nParticleServer* self = (nParticleServer*) slf;
    self->SetEnabled(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isenabled
    @input
    v
    @output
    b(Enabled)
    @info
    Get enabled state of the particle subsystem.
*/
static void
n_isenabled(void* slf, nCmd* cmd)
{
    nParticleServer* self = (nParticleServer*) slf;
    cmd->Out()->SetB(cmd->In()->GetB());
}

