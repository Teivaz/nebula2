//------------------------------------------------------------------------------
//  nparticleserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleserver.h"

static void n_setenabled(void* slf, nCmd* cmd);
static void n_isenabled(void* slf, nCmd* cmd);
static void n_setglobalacceleration(void* slf, nCmd* cmd);
static void n_getglobalacceleration(void* slf, nCmd* cmd);

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
    cl->AddCmd("v_setenabled_b",              'SENB', n_setenabled);
    cl->AddCmd("b_isenabled_v",               'GENB', n_isenabled);
    cl->AddCmd("v_setglobalacceleration_fff", 'SGAC', n_setglobalacceleration);
    cl->AddCmd("fff_getglobalacceleration_v", 'GGAC', n_getglobalacceleration);
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

//------------------------------------------------------------------------------
/**
    @cmd
    setglobalacceleration
    @input
    f(acceleration vector)
    @output
    v
    @info
    Every particle will have this base "gravitational" acceleration, multiplied
    by the particle's weight (in addition to the wind and whatever the effect
    may specify).
*/
static void
n_setglobalacceleration(void* slf, nCmd* cmd)
{
    nParticleServer* self = (nParticleServer*) slf;
    vector3 globalAccel;
    globalAccel.x = cmd->In()->GetF();
    globalAccel.y = cmd->In()->GetF();
    globalAccel.z = cmd->In()->GetF();
    self->SetGlobalAccel(globalAccel);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getglobalacceleration
    @input
    v
    @output
    fff(acceleration vector)
    @info
    The base "gravitational" acceleration of the particle system.
*/
static void
n_getglobalacceleration(void* slf, nCmd* cmd)
{
    nParticleServer* self = (nParticleServer*) slf;
    const vector3& globalAccel = self->GetGlobalAccel();
    cmd->Out()->SetF(globalAccel.x);
    cmd->Out()->SetF(globalAccel.y);
    cmd->Out()->SetF(globalAccel.z);
}





