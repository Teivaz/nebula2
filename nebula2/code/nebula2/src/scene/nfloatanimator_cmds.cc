//------------------------------------------------------------------------------
//  nfloatanimator_cmds.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "scene/nfloatanimator.h"
#include "kernel/npersistserver.h"

static void n_addkey(void* slf, nCmd* cmd);
static void n_getnumkeys(void* slf, nCmd* cmd);
static void n_getkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfloatanimator
    @cppclass
    nFloatAnimator
    @superclass
    nshaderanimator
    @classinfo
    Animate a float attribute of a nabstractshadernode.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addkey_ff",    'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v", 'GNKS', n_getnumkeys);
    cl->AddCmd("ff_getkeyat_i",  'GKAT', n_getkeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey
    @input
    f(Time), f(Value)
    @output
    v
    @info
    Add a float key to the animation key array.
*/
void
n_addkey(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    float f0, f1;
    f0 = cmd->In()->GetF();
    f1 = cmd->In()->GetF();
    self->AddKey(f0, f1);
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
    nFloatAnimator* self = (nFloatAnimator*) slf;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyat
    @input
    i(KeyIndex)
    @output
    f(Time), f(Value)
    @info
    Returns key at given index.
*/
void
n_getkeyat(void* slf, nCmd* cmd)
{
    nFloatAnimator* self = (nFloatAnimator*) slf;
    float f0, f1;
    self->GetKeyAt(cmd->In()->GetI(), f0, f1);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetF(f1);
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatAnimator::SaveCmds(nPersistServer* ps)
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
            float val;
            cmd = ps->GetCmd(this, 'ADDK');
            this->GetKeyAt(i, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetF(val);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}

