//------------------------------------------------------------------------------
//  naudioserver3_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio3/naudioserver3.h"

static void n_reset(void* slf, nCmd* cmd);
static void n_setmastervolume(void* slf, nCmd* cmd);
static void n_getmastervolume(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    naudioserver3
    
    @cppclass
    nAudioServer3
    
    @superclass
    nroot

    @classinfo
    Server object of the Nebula2 audio subsystem (in its 3rd incarnation).
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_reset_v",           'RSET', n_reset);
    cl->AddCmd("v_setmastervolume_f", 'SMSV', n_setmastervolume);
    cl->AddCmd("f_getmastervolume_v", 'GMSV', n_getmastervolume);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    reset
    @input
    v
    @output
    v
    @info
    Stop all playing sounds.
*/
static void
n_reset(void* slf, nCmd* cmd)
{
    nAudioServer3* self = (nAudioServer3*) slf;
    self->Reset();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setmastervolume
    @input
    f(MasterVolume)
    @output
    v
    @info
    Set master volume (0.0 .. 1.0)
*/
static void
n_setmastervolume(void* slf, nCmd* cmd)
{
    nAudioServer3* self = (nAudioServer3*) slf;
    self->SetMasterVolume(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getmastervolume
    @input
    v
    @output
    f(MasterVolume)
    @info
    Get master volume (0.0 .. 1.0)
*/
static void
n_getmastervolume(void* slf, nCmd* cmd)
{
    nAudioServer3* self = (nAudioServer3*) slf;
    cmd->Out()->SetF(self->GetMasterVolume());
}




