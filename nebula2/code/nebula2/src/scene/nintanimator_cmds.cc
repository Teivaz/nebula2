//------------------------------------------------------------------------------
//  nintanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nintanimator.h"
#include "kernel/npersistserver.h"

static void n_setintname(void* slf, nCmd* cmd);
static void n_getintname(void* slf, nCmd* cmd);
static void n_addkey(void* slf, nCmd* cmd);
static void n_getnumkeys(void* slf, nCmd* cmd);
static void n_getkeyat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nintanimator    

    @cppclass
    nIntAnimator

    @superclass
    nanimator

    @classinfo
    Animate an int attribute of a nabstractshadernode.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setintname_s",    'SVCN', n_setintname);
    cl->AddCmd("s_getintname_v",    'GVCN', n_getintname);
    cl->AddCmd("v_addkey_fi",       'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v",    'GNKS', n_getnumkeys);
    cl->AddCmd("fi_getkeyat_i",     'GKAT', n_getkeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setintname
    @input
    s(IntName)
    @output
    v
    @info
    Set name of int variable to animate in target object.
*/
void
n_setintname(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    self->SetIntName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getintname
    @input
    v
    @output
    s(IntName)
    @info
    Get name of int variable to animate in target object.
*/
void
n_getintname(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    cmd->Out()->SetS(self->GetIntName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    addkey
    @input
    f(Time), i(Value)
    @output
    v
    @info
    Add a int key to the animation key array.
*/
void
n_addkey(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    float f0;
    int i0;
    f0 = cmd->In()->GetF();
    i0 = cmd->In()->GetI();

    self->AddKey(f0, i0);
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
    nIntAnimator* self = (nIntAnimator*) slf;
    cmd->Out()->SetI(self->GetNumKeys());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getkeyat
    @input
    i(KeyIndex)
    @output
    f(Time), i(Value)
    @info
    Returns key at given index.
*/
void
n_getkeyat(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    float f0;
    int i0;
    self->GetKeyAt(cmd->In()->GetI(), f0, i0);
    cmd->Out()->SetF(f0);
    cmd->Out()->SetI(i0);
}

//------------------------------------------------------------------------------
/**
*/
bool
nIntAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setintname ---
        if (this->GetIntName())
        {
            cmd = ps->GetCmd(this, 'SVCN');
            cmd->In()->SetS(this->GetIntName());
            ps->PutCmd(cmd);
        }

        //--- addkey ---
        int i;
        int numKeys = this->GetNumKeys();
        for (i = 0; i < numKeys; i++)
        {
            float time;
            int val;
            cmd = ps->GetCmd(this, 'ADDK');
            this->GetKeyAt(i, time, val);
            cmd->In()->SetF(time);
            cmd->In()->SetI(val);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
