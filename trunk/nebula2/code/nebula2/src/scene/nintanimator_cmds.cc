//------------------------------------------------------------------------------
//  nintanimator_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nintanimator.h"
#include "kernel/npersistserver.h"

static void n_setvectorname(void* slf, nCmd* cmd);
static void n_getvectorname(void* slf, nCmd* cmd);
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
    Animate a int vector attribute of a nabstractshadernode.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setvectorname_s", 'SVCN', n_setvectorname);
    cl->AddCmd("s_getvectorname_v", 'GVCN', n_getvectorname);
    cl->AddCmd("v_addkey_fi",    'ADDK', n_addkey);
    cl->AddCmd("i_getnumkeys_v",    'GNKS', n_getnumkeys);
    cl->AddCmd("fi_getkeyat_i",  'GKAT', n_getkeyat);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setvectorname
    @input
    s(VectorName)
    @output
    v
    @info
    Set name of int vector variable to animate in target object.
*/
void
n_setvectorname(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    self->SetVectorName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getvectorname
    @input
    v
    @output
    s(VectorName)
    @info
    Get name of int vector variable to animate in target object.
*/
void
n_getvectorname(void* slf, nCmd* cmd)
{
    nIntAnimator* self = (nIntAnimator*) slf;
    cmd->Out()->SetS(self->GetVectorName());
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
    Add a int vector key to the animation key array.
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

        //--- setvectorname ---
        if (this->GetVectorName())
        {
            cmd = ps->GetCmd(this, 'SVCN');
            cmd->In()->SetS(this->GetVectorName());
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
