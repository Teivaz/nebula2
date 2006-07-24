//------------------------------------------------------------------------------
//  nvectoranimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nvectoranimator.h"
#include "kernel/npersistserver.h"

static void n_addkey(void* slf, nCmd* cmd);
static void n_getnumkeys(void* slf, nCmd* cmd);
static void n_getkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nvectoranimator

    @cppclass
    nVectorAnimator

    @superclass
    nshaderanimator

    @classinfo
    Animate a vector attribute of a nabstractshadernode.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addkey_fffff",    'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v",    'GNKS', n_getnumkeys);
    cl->AddCmd("fffff_getkeyat_i",  'GKAT', n_getkeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey
    @input
    f(Time), f(X), f(Y), f(Z), f(W)
    @output
    v
    @info
    Add a vector key to the animation key array.
*/
void
n_addkey(void* slf, nCmd* cmd)
{
    nVectorAnimator* self = (nVectorAnimator*) slf;
    float f0;
    vector4 v0;
    f0   = cmd->In()->GetF();
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    v0.w = cmd->In()->GetF();
    self->AddKey(f0, v0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumkeys
    @input
    v
    @output
    i(NumKeys)
    @info
    Returns number of key in animation array.
*/
void
n_getnumkeys(void* slf, nCmd* cmd)
{
    nVectorAnimator* self = (nVectorAnimator*) slf;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyat
    @input
    i(KeyIndex)
    @output
    f(Time), f(X), f(Y), f(Z), f(W)
    @info
    Returns key at given index.
*/
void
n_getkeyat(void* slf, nCmd* cmd)
{
    nVectorAnimator* self = (nVectorAnimator*) slf;
    float f0;
    vector4 v0;
    self->GetKeyAt(cmd->In()->GetI(), f0, v0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(v0.x);
    cmd->Out()->SetF(v0.y);
    cmd->Out()->SetF(v0.z);
    cmd->Out()->SetF(v0.w);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVectorAnimator::SaveCmds(nPersistServer* ps)
{
    if (nShaderAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- addkey ---
        int i;
        int numKeys = this->GetNumKeys();
        for (i = 0; i < numKeys; i++)
        {
            float time;
            vector4 val;
            cmd = ps->GetCmd(this, 'ADDK');
            this->GetKeyAt(i, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(val.x);
            cmd->In()->SetF(val.y);
            cmd->In()->SetF(val.z);
            cmd->In()->SetF(val.w);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
